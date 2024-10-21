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

	FCollisionQueryParams GroundCollisionQueryParams;
	FCalculateCustomPhysics CalculateCustomPhysics;

	void CustomPhysicsSubstepTick(float DeltaTime, FBodyInstance* BodyInstance);
};
