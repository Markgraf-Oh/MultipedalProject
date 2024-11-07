// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultipedalData.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Misc/Optional.h"
#include "MultipedalMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPEDALPROJECT_API UMultipedalMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

	UMultipedalMovementComponent();
	
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void InitializeMultipedalData(class UMultipedalDataAsset* MultipedalData);
	void ActivateLocalControl(bool bInActivate);

private:
	TOptional<bool> bLocalControl;

	UPROPERTY(VisibleAnywhere)
	TArray<FPedal> PedalList;

	FVector COMOffset;
	float TotalMassKg = 1000.0f;

	// cm/s^2
	const float GravityAccel = 980.0f;
	constexpr float GetConstGravityAccel() { return GravityAccel; }

	FVector GetGravityVector()
	{
		return GetGravityZ() * FVector::ZAxisVector;
	}

	FCollisionQueryParams GroundCollisionQueryParams;

	//PhysicsTickValue
	FVector TotalForce;
	FVector TotalTorque;
	FVector LinearVelocity;
	FVector AngularVelocity;
	FVector COMWorldLocation;
	FTransform BodyTransform;

	//PhysicsTick
	FCalculateCustomPhysics CalculateCustomPhysics;
	void CustomPhysicsSubstepTick(float DeltaTime, FBodyInstance* BodyInstance);

	void BeginForceCalc(float DeltaTime, FBodyInstance* BodyInstance);
	void CalcVerticalSuspentionForce(float DeltaTime, FBodyInstance* BodyInstance);
	void ApplyForceCalc(float DeltaTime, FBodyInstance* BodyInstance);
};
