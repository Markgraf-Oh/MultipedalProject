// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MultipedalPawn.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPEDALPROJECT_API AMultipedalPawn : public APawn
{
	GENERATED_BODY()
	
public:
	AMultipedalPawn();

	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleDefaultsOnly)
	class USkeletalMeshComponent* BodyMesh;

	UPROPERTY(VisibleDefaultsOnly)
	class UMultipedalEquipComponent* EquipComp;
};
