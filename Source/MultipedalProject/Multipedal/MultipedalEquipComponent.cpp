// Fill out your copyright notice in the Description page of Project Settings.


#include "MultipedalEquipComponent.h"
#include "MultipedalData.h"
#include "Engine/AssetManager.h"
#include "Components/SkeletalMeshComponent.h"

UMultipedalEquipComponent::UMultipedalEquipComponent()
{

}

void UMultipedalEquipComponent::BeginPlay()
{
	Super::BeginPlay();
	
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
			}
		}
	}
}
