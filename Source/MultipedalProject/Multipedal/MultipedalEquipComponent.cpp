// Fill out your copyright notice in the Description page of Project Settings.


#include "MultipedalEquipComponent.h"
#include "MultipedalData.h"
#include "Engine/AssetManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "MultipedalPawn.h"
#include "MultipedalMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Engine/CollisionProfile.h"

UMultipedalEquipComponent::UMultipedalEquipComponent()
{
	bWantsInitializeComponent = true;
}

void UMultipedalEquipComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (AMultipedalPawn* OwningPawn = Cast<AMultipedalPawn>(GetOwner()))
	{
		if (IsValid(OwningPawn->GetPlayerState()))
		{
			OnPlayerStateChanged(OwningPawn->GetPlayerState());
		}
		OwningPawn->OnPlayerStateChangedNotify.AddUObject(this, &ThisClass::OnPlayerStateChanged);
	}
}

void UMultipedalEquipComponent::BeginPlay()
{
	Super::BeginPlay();
	//Possess ���� �� Test �ڵ�
	EquipBodyAsset(TEXT("Test"));
}

void UMultipedalEquipComponent::OnPlayerStateChanged(class APlayerState* PlayerState)
{
	//������� PlayerState�� Component ������ ���� ���������� ������ ȹ���� �� �����ϰ�����, �켱�� �ӽ� ���� ����
	//EquipBodyAsset(TEXT("Test"));	
}

void UMultipedalEquipComponent::EquipBodyAsset(FString ItemId)
{
	if (ensure(UAssetManager::IsInitialized()))
	{
		TArray<FName> Bundles;
		Bundles.Add(TEXT("Visual"));
		Bundles.Add(TEXT("Animation"));

		FPrimaryAssetId MultipedalPrimaryAssetId = FPrimaryAssetId(FPrimaryAssetType(FName(TEXT("UMultipedalDataAsset"))), MultipedalBodyId);
		BodyAssetLoadHandle = UAssetManager::Get().LoadPrimaryAsset(MultipedalPrimaryAssetId, Bundles, FStreamableDelegate::CreateUObject(this, &ThisClass::PostBodyAssetLoad));
	}
}

void UMultipedalEquipComponent::PostBodyAssetLoad()
{
	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		FPrimaryAssetId MultipedalPrimaryAssetId = FPrimaryAssetId(FPrimaryAssetType(FName(TEXT("UMultipedalDataAsset"))), MultipedalBodyId);
		MultipedalDataAsset = AssetManager->GetPrimaryAssetObject<UMultipedalDataAsset>(MultipedalPrimaryAssetId);
		if (IsValid(MultipedalDataAsset))
		{
			USkeletalMeshComponent* OwnerSkeletal = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
			if (IsValid(OwnerSkeletal) && MultipedalDataAsset->Mesh.IsValid())
			{
				OwnerSkeletal->SetSkeletalMesh(MultipedalDataAsset->Mesh.Get());
				OwnerSkeletal->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
				OwnerSkeletal->SetSimulatePhysics(true);
			}

			UMultipedalMovementComponent* Movement = GetOwner()->FindComponentByClass<UMultipedalMovementComponent>();
			if (IsValid(Movement))
			{
				Movement->InitializeMultipedalData(MultipedalDataAsset);
				Movement->ActivateLocalControl(true);
			}
		}
	}
}
