// Fill out your copyright notice in the Description page of Project Settings.


#include "MultipedalMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/EngineTypes.h"

UMultipedalMovementComponent::UMultipedalMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMultipedalMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(UpdatedPrimitive) && UpdatedPrimitive->GetBodyInstance())
	{
		if (bLocalControl.IsSet() && bLocalControl.GetValue())
		{
			CalculateCustomPhysics.BindUObject(this, &ThisClass::CustomPhysicsSubstepTick);
			UpdatedPrimitive->GetBodyInstance()->AddCustomPhysics(CalculateCustomPhysics);
		}
	}
}

void UMultipedalMovementComponent::InitializeMultipedalData(class UMultipedalDataAsset* MultipedalData)
{
	if (!IsValid(MultipedalData))
	{
		return;
	}
	PedalList.Empty();
	PedalList.SetNum(MultipedalData->PedalDefList.Num());

	GroundCollisionQueryParams = FCollisionQueryParams();
	GroundCollisionQueryParams.bTraceComplex = true;
	GroundCollisionQueryParams.bReturnFaceIndex = false;
	GroundCollisionQueryParams.bReturnPhysicalMaterial = false;
	GroundCollisionQueryParams.AddIgnoredActor(GetOwner());

	for (int32 PedalIndex = 0; PedalIndex < PedalList.Num(); PedalIndex++)
	{
		FPedal& CurrentPedal = PedalList[PedalIndex];
		CurrentPedal.Def = MultipedalData->PedalDefList[PedalIndex];
		FVector InitialFootWorldLoc = UKismetMathLibrary::TransformLocation(GetOwner()->GetActorTransform(), CurrentPedal.Def.InitialRelativeLocation);
		
		FVector Upward = FVector::UpVector;
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, InitialFootWorldLoc + Upward * CurrentPedal.Def.UpwardRange, InitialFootWorldLoc - Upward * CurrentPedal.Def.DownwardRange, ECollisionChannel::ECC_PhysicsBody, GroundCollisionQueryParams);
		
		if (Hit.bBlockingHit)
		{
			CurrentPedal.MovementState = EPedalState::OnSurface;
			CurrentPedal.StartLocation = Hit.Location;
			CurrentPedal.TargetLocation = Hit.Location;
		}
		else
		{
			CurrentPedal.MovementState = EPedalState::Hanging;
			CurrentPedal.StartLocation = InitialFootWorldLoc;
			CurrentPedal.TargetLocation = InitialFootWorldLoc;
		}
	}
}

void UMultipedalMovementComponent::ActivateLocalControl(bool bInActivate)
{
	if (!IsValid(UpdatedPrimitive) || !UpdatedPrimitive->GetBodyInstance())
	{
		return;
	}

	if (!bLocalControl.IsSet() || bLocalControl.GetValue() != bInActivate)
	{
		bLocalControl = bInActivate;
		SetComponentTickEnabled(bLocalControl.GetValue());
	}
}

void UMultipedalMovementComponent::CustomPhysicsSubstepTick(float DeltaTime, FBodyInstance* BodyInstance)
{
	DrawDebugBox(GetWorld(), BodyInstance->GetCOMPosition(), FVector(2.0f), FQuat::Identity, FColor::Red);
}
