// Fill out your copyright notice in the Description page of Project Settings.


#include "MultipedalPawn.h"
#include "MultipedalEquipComponent.h"
#include "MultipedalMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerState.h"

AMultipedalPawn::AMultipedalPawn()
{
	//SkeletalMesh is not facing front side. So use boxcomponent as root primitive;
	BodyCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyCollisionBox"));
	SetRootComponent(BodyCollisionBox);
	BodyCollisionBox->bAlwaysCreatePhysicsState = true;
	

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));	
	SkeletalMesh->bAlwaysCreatePhysicsState = true;
	SkeletalMesh->SetupAttachment(BodyCollisionBox);
	SkeletalMesh->PhysicsTransformUpdateMode = EPhysicsTransformUpdateMode::ComponentTransformIsKinematic;

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
