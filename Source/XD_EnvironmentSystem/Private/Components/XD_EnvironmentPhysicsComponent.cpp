// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/XD_EnvironmentPhysicsComponent.h"
#include "XD_EnvironmentManager.h"
#include "XD_EnvironmentSystemUtility.h"

UXD_EnvironmentPhysicsComponent::UXD_EnvironmentPhysicsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bRunOnAnyThread = true;

}


void UXD_EnvironmentPhysicsComponent::BeginPlay()
{
	Super::BeginPlay();

	EnvironmentManager = UXD_EnvironmentManager::GetManager(this);

	AActor* Owner = GetOwner();
	bool Simulatable = false;
	if (Owner->GetRootComponent()->IsSimulatingPhysics())
	{
		if (Owner->GetIsReplicated())
		{
			if (!Owner->HasAuthority())
			{
				Simulatable = false;
			}
		}
		else
		{
			Simulatable = true;
		}
	}
	SetComponentTickEnabled(Simulatable);
}

DECLARE_CYCLE_STAT(TEXT("WindPhysicsSimulate"), STAT_WindPhysicsSimulate, STATGROUP_ENVIRONMENTSYSTEM);

void UXD_EnvironmentPhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent()))
	{
		if (Root->IsSimulatingPhysics())
		{
			// 风物理模拟
			{
				SCOPE_CYCLE_COUNTER(STAT_WindPhysicsSimulate);
				const FVector CurrentVelocity = Root->GetComponentVelocity();
				const FTransform WorldTransform = Root->GetComponentTransform();
				for (const FVector& TestPoint : WindTestPoint)
				{
					const FVector WorldTestPoint = WorldTransform.TransformPosition(TestPoint);
					const FVector WindVelocity = EnvironmentManager->GetWindVelocity(WorldTestPoint);
					const FVector Force = (WindVelocity - CurrentVelocity) * WindEffectScale * DeltaTime;
					Root->AddForceAtLocation(Force, WorldTestPoint);
				}
			}
		}
	}
}

