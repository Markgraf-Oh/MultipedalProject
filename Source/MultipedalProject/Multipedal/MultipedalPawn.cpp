// Fill out your copyright notice in the Description page of Project Settings.


#include "MultipedalPawn.h"
#include "MultipedalEquipComponent.h"
#include "MultipedalMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerState.h"

AMultipedalPawn::AMultipedalPawn()
{
	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	SetRootComponent(BodyMesh);
	BodyMesh->bAlwaysCreatePhysicsState = true;

	EquipComp = CreateDefaultSubobject<UMultipedalEquipComponent>(TEXT("Equip"));

	MovementComp = CreateDefaultSubobject<UMultipedalMovementComponent>(TEXT("MultipedalMovement"));
}

void AMultipedalPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AMultipedalPawn::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);

	if (IsValid(NewPlayerState))
	{
		OnPlayerStateChangedNotify.Broadcast(NewPlayerState);
	}
}
