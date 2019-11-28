// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XD_SaveGameInterface.h"
#include "XD_EnvironmentManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XD_ENVIRONMENTSYSTEM_API UXD_EnvironmentManager : public UActorComponent, public IXD_SaveGameInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UXD_EnvironmentManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnRegister() override;

	virtual void OnUnregister() override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const override;

	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
public:
#if WITH_EDITOR
	static TWeakObjectPtr<UXD_EnvironmentManager> PreviewEnvironmentManager;
#endif

	static UXD_EnvironmentManager* GetManager(const UObject* WorldContextObject);
	
	FDelegateHandle OnActorSpawnedHandle;

	UPROPERTY(EditAnywhere, Category = "环境系统")
	TArray<AActor*> SubManagerActors;

	UPROPERTY(EditAnywhere, Category = "环境系统", SaveGame, Instanced)
	TArray<class UXD_EnvironmentSubManager*> SubManagers;
	//气候
public:
	UPROPERTY(EditAnywhere, Replicated, SaveGame, meta = (DisplayName = "湿度"), Category = "环境系统")
	float Humidity = 0.1f;

	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统")
	float GetHumidity(const FVector& Position) const { return Humidity; }

	UPROPERTY(EditAnywhere, Replicated, SaveGame, meta = (DisplayName = "气温"), Category = "环境系统")
	float Temperature = 20.f;

	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统")
	float GetTemperature(const FVector& Position) const { return Temperature; }

	// 风
public:
	/*
		| 风力等级  | 风的名称 | 风速（m/s） | 风速（Km/h） | 海岸渔船象征                        | 陆地状况                                   | 海面状况 |
		| -------- | -------- | ----------- | ------------ | ----------------------------------- | ------------------------------------------ | -------- |
		| 0        | 无风     | 0~0.2       | 小于1        | 静                                  | 静，烟直上                                 | 平静无感 |
		| 1        | 软风     | 0.3~1.5     | 1~5          | 寻常渔船略觉摇动                    | 烟能表示风向，但风向标不能转动             | 微浪     |
		| 2        | 轻风     | 1.6~3.3     | 6~11         | 渔船张帆时，可随风移行每小时2~3km   | 人面感觉有风，树叶有微响，风向标能转动     | 小浪     |
		| 3        | 微风     | 3.4~5.4     | 12~19        | 渔船渐觉簸动，可随风移行每小时5~6km | 树叶及微枝摆动不息，旗帜展开               | 小浪     |
		| 4        | 和风     | 5.5~7.9     | 20~28        | 渔船满帆时，倾于一方                | 能吹起地面灰尘和纸张，树的小枝微动         | 轻浪     |
		| 5        | 清劲风   | 8.0~10.7    | 29~38        | 渔船缩帆（即收去帆之一部）          | 有叶的小树枝摇摆，内陆水面有小波           | 中浪     |
		| 6        | 强风     | 10.8~13.8   | 39~49        | 渔船加倍缩帆，捕鱼需注意风险        | 大树枝摆动，电线呼呼有声，举伞困难         | 大浪     |
		| 7        | 疾风     | 13.9~17.1   | 50~61        | 渔船停息港中，在海上下锚            | 全树摇动，迎风步行感觉不便                 | 巨浪     |
		| 8        | 大风     | 17.2~20.7   | 62~74        | 进港的渔船皆停留不出                | 微枝折毁，人向前行感觉阻力甚大             | 猛浪     |
		| 9        | 烈风     | 20.8~24.4   | 75~88        | 汽船航行困难                        | 建筑物有损坏（烟囱顶部及屋顶瓦片移动）     | 狂涛     |
		| 10       | 狂风     | 24.5~28.4   | 89~102       | 汽船航行颇危险                      | 陆上少见，见时可使树木拔起将建筑物损坏严重 | 狂涛     |
		| 11       | 暴风     | 28.5~32.6   | 103~117      | 汽船遇之极危险                      | 陆上很少，有则必有重大损毁                 | 非凡现象 |
		| 12       | 飓风     | 32.7~36.9   | 118~133      | 海浪滔天                            | 陆上绝少，其摧毁力极大                     | 非凡现象 |
	*/
	UPROPERTY(EditAnywhere, Replicated, SaveGame, meta = (DisplayName = "风速"), Category = "环境系统")
	FVector GlobalWindVelocity = FVector(0.f, 300.f, 0.f);
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "游戏|环境系统")
	virtual FVector GetWindVelocity(const FVector& Position) const;
	static TOptional<FVector> SampleVectorField(class UVectorFieldComponent* VectorFieldComponent, const FVector& Position);

	UPROPERTY(Transient)
	TArray<UVectorFieldComponent*> WindVectorFields;
	UFUNCTION()
	void WhenVectorFieldDestroyed(AActor* VectorField);

	float GetGlobalWindSpeed() const { return GlobalWindVelocity.Size(); }
	void SetGlobalWindSpeed(float InWindSpeed);

	float GetWindFieldIntensity() const { return GetGlobalWindSpeed() / 20.f; }
	//云
public:
	UPROPERTY(EditAnywhere, Replicated, SaveGame, meta = (DisplayName = "云密度", ClampMin = "0", ClampMax = "1"), Category = "环境系统")
	float CloudsDensity = 0.2f;

	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统")
	float GetCloudsDensity(const FVector& Position) const { return CloudsDensity; }

	// 雾
public:
	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统")
	float GetFogDensity(const FVector& Position) const { return Humidity; }

	// 降雨
public:
	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统")
	float GetRainfall(const FVector& Position) const { return 0.f; }

	// 降雪
public:
	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统")
	float GetSnowfall(const FVector& Position) const { return 0.f; }

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "环境系统")
	class AXD_InsideManagerActor* InsideManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "环境系统")
	class AXD_SnowPathManagerActorBase* SnowPathManager;
};

UCLASS(abstract, Blueprintable, BlueprintType, EditInlineNew, collapsecategories)
class XD_ENVIRONMENTSYSTEM_API UXD_EnvironmentSubManager : public UObject
{
	GENERATED_BODY()
public:
	virtual class UWorld* GetWorld() const override;

	virtual bool IsSupportedForNetworking() const override { return true; }
public:
	UFUNCTION(BlueprintCallable, Category = "环境系统")
	class AActor* GetOwner() const;

	UFUNCTION(BlueprintNativeEvent, Category = "环境系统", meta = (DisplayName = "OnConstruction"))
	void ReceiveOnConstruction(const FTransform& Transform);
	virtual void ReceiveOnConstruction_Implementation(const FTransform& Transform) {}

	UFUNCTION(BlueprintNativeEvent, Category = "环境系统", meta = (DisplayName = "BeginPlay"))
	void ReceiveBeginPlay();
	virtual void ReceiveBeginPlay_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "环境系统", meta = (DisplayName = "Tick"))
	void ReceiveTick(float DeltaSeconds);
	virtual void ReceiveTick_Implementation(float DeltaSeconds) {}

	UFUNCTION(BlueprintNativeEvent, Category = "环境系统")
	void WhenFreshClick();
	virtual void WhenFreshClick_Implementation() {}
};
