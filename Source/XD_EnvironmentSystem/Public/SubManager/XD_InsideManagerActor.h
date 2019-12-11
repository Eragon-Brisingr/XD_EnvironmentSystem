// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubManager/XD_SubManagerActorBase.h"
#include "XD_InsideManagerActor.generated.h"

UCLASS()
class XD_ENVIRONMENTSYSTEM_API AXD_InsideManagerActor : public AXD_SubManagerActorBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AXD_InsideManagerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
