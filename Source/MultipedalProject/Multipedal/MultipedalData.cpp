// Fill out your copyright notice in the Description page of Project Settings.


#include "MultipedalData.h"
#include "Curves/CurveFloat.h"
#include "Engine/AssetManager.h"
#if WITH_EDITOR
#include "Animation/SkeletalMeshActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Editor/EditorEngine.h"
#endif

FPrimaryAssetId UMultipedalDataAsset::GetPrimaryAssetId() const
{
	if (ItemID == NAME_None)
	{
		return FPrimaryAssetId(FPrimaryAssetType(FName(TEXT("UMultipedalDataAsset"))), GetFName());
	}
	return FPrimaryAssetId(FPrimaryAssetType(FName(TEXT("UMultipedalDataAsset"))), ItemID);
}

void UMultipedalDataAsset::UpdateAssetBundleData()
{
	if (UAssetManager::IsInitialized())
	{
		AssetBundleData.Reset();

		for (FPedalDefinition& PedalDef : PedalDefList)
		{
			FTopLevelAssetPath VerticalCurvePath = PedalDef.VerticalCurve.ToSoftObjectPath().GetAssetPath();
			if (VerticalCurvePath.IsValid())
			{
				AssetBundleData.AddBundleAsset(TEXT("Animation"), VerticalCurvePath);
			}

			FTopLevelAssetPath ProgressCurvePath = PedalDef.ProgressCurve.ToSoftObjectPath().GetAssetPath();
			if (ProgressCurvePath.IsValid())
			{
				AssetBundleData.AddBundleAsset(TEXT("Animation"), ProgressCurvePath);
			}
		}

		UAssetManager::Get().InitializeAssetBundlesFromMetadata(this, AssetBundleData);
		//프라이머리 에셋과 로드된 번들은 AssetManager에서 참조가 유지된다.
	}
}

#if WITH_EDITOR
void UMultipedalDataAsset::MeasurePedalProperties()
{
	if (!GEditor || !IsValid(GEditor->GetEditorWorldContext().World()))
	{
		return;
	}

	USkeletalMesh* LoadedMesh = Mesh.LoadSynchronous();
	if (!IsValid(LoadedMesh))
	{
		return;
	}

	AActor* MeasureActor = GEditor->GetEditorWorldContext().World()->SpawnActor<AActor>();	
	USkeletalMeshComponent* MeshComp = NewObject<USkeletalMeshComponent>(MeasureActor);
	MeshComp->RegisterComponent();
	MeshComp->SetupAttachment(MeasureActor->GetRootComponent());
	MeshComp->SetRelativeTransform(MeshRelativeTransform);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->PhysicsTransformUpdateMode = EPhysicsTransformUpdateMode::ComponentTransformIsKinematic;
	MeshComp->SetEnableGravity(false);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetSkeletalMesh(LoadedMesh);

	for (FPedalDefinition& PedalDef : PedalDefList)
	{
		FVector InitialWorldLocation = MeshComp->GetBoneLocation(PedalDef.EndBoneName, EBoneSpaces::WorldSpace);
		PedalDef.InitialRelativeLocation = UKismetMathLibrary::InverseTransformLocation(MeasureActor->GetActorTransform(), InitialWorldLocation);
		FVector RootWorldLocation = MeshComp->GetBoneLocation(PedalDef.RootBoneName, EBoneSpaces::WorldSpace);
		PedalDef.RootRelativeLocation = UKismetMathLibrary::InverseTransformLocation(MeasureActor->GetActorTransform(), RootWorldLocation);
		
		UE_LOG(LogTemp, Warning, TEXT("Initial : %s / Root : %s"), *InitialWorldLocation.ToString(), *RootWorldLocation.ToString());
	}
}

FVector UMultipedalDataAsset::GetBoneDefaultLocation(USkeletalMesh* SkeletalMesh, FName BoneName)
{
	int32 BoneIndex = INDEX_NONE;
	if (BoneName != NAME_None && SkeletalMesh)
	{
		BoneIndex = SkeletalMesh->GetRefSkeleton().FindBoneIndex(BoneName);
		TArray<FTransform> BonePose = SkeletalMesh->GetRefSkeleton().GetRefBonePose();
		if (BonePose.IsValidIndex(BoneIndex))
		{
			return BonePose[BoneIndex].GetLocation();
		}
	}
	return FVector::ZeroVector;
}
#endif
