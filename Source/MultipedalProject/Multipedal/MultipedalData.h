// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UObject/SoftObjectPtr.h"
#include "MultipedalData.generated.h"

USTRUCT()
struct FPedalDefinition
{
	GENERATED_BODY()

	UPROPERTY()
	FName EndBoneName;

	UPROPERTY()
	float UpwardRange;
	 
	UPROPERTY()
	float DownwardRange;

	UPROPERTY()
	float MoveThreshold;

	UPROPERTY()
	FVector InitialRelativeLocation;

	UPROPERTY()
	TSoftObjectPtr<class UCurveFloat> VerticalCurve;

	UPROPERTY()
	TSoftObjectPtr<class UCurveFloat> ProgressCurve;
};

UENUM()
enum class EPedalState
{
	//지표면에 고정된 상태
	OnSurface,
	//이동을 위해 의도적으로 공중에서 움직이고 있는 상태
	Stepping,
	//이동 목표지점이 무효화 되었거나, 외부요인으로인해 땅으로부터 멀어져 공중에 떠있는 상태
	Hanging
};

//각 다리의 Instance
USTRUCT()
struct FPedal
{
	GENERATED_BODY()

	UPROPERTY()
	FPedalDefinition Def;

	UPROPERTY()
	EPedalState MovementState;

	UPROPERTY()
	bool bOnWall;

	UPROPERTY()
	FVector StartLocation;

	UPROPERTY()
	FVector TargetLocation;
};

/**
 * 
 */
UCLASS()
class MULTIPEDALPROJECT_API UMultipedalDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif

	//load하기 위한 쿼리 ID, ItemID와 같다.
	UPROPERTY(EditAnywhere, Category = "PrimaryDataAsset")
	FName ItemID = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category = "Asset", meta = (AssetBundles = "Visual"))
	TSoftObjectPtr<class USkeletalMesh> Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Asset", meta = (AssetBundles = "Animation"))
	TSoftClassPtr<class UAnimInstance> Anim;

	UPROPERTY(EditDefaultsOnly, Category = "Pedal")
	TArray<FPedalDefinition> PedalDefList;
};
