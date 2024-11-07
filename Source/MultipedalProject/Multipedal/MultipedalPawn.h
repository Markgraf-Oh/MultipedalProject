// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MultipedalPawn.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, class APlayerState*);

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
	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;

public:
	FOnPlayerStateChanged OnPlayerStateChangedNotify;

protected:
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class UBoxComponent> BodyCollisionBox;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class UMultipedalEquipComponent> EquipComp;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class UMultipedalMovementComponent> MovementComp;
};
