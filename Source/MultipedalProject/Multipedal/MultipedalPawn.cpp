// Fill out your copyright notice in the Description page of Project Settings.


#include "MultipedalPawn.h"
#include "MultipedalEquipComponent.h"
#include "Components/SkeletalMeshComponent.h"

AMultipedalPawn::AMultipedalPawn()
{
	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	SetRootComponent(BodyMesh);

	EquipComp = CreateDefaultSubobject<UMultipedalEquipComponent>(TEXT("Equip"));
}

void AMultipedalPawn::BeginPlay()
{
	Super::BeginPlay();

	
}
