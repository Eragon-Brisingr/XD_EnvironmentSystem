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
	UPROPERTY(EditAnywhere, Replicated, SaveGame, meta = (DisplayName = "风速"), Category = "环境系统")
	FVector WindVelocity = FVector(1.f, 0.f, 0.f);

	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统")
	FVector GetWindVelocity(const FVector& Position) const { return WindVelocity; }
	
	UFUNCTION(BlueprintCallable, Category = "游戏|环境系统")
	float GetWindSpeed(const FVector& Position) const { return WindVelocity.Size(); }

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
