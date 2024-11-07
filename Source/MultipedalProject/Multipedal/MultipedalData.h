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

	UPROPERTY(EditDefaultsOnly)
	FName EndBoneName;

	UPROPERTY(EditDefaultsOnly)
	FName RootBoneName;

	//BodyInstance에 대한 상대위치
	UPROPERTY(EditDefaultsOnly)
	FVector InitialRelativeLocation;

	//BodyInstance에 대한 상대위치
	UPROPERTY(EditDefaultsOnly)
	FVector RootRelativeLocation;

	UPROPERTY(EditDefaultsOnly)
	float UpwardRange;
	 
	UPROPERTY(EditDefaultsOnly)
	float DownwardRange;

	UPROPERTY(EditDefaultsOnly)
	float MoveThreshold;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<class UCurveFloat> VerticalCurve;

	UPROPERTY(EditDefaultsOnly)
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

	UPROPERTY(VisibleAnywhere)
	FPedalDefinition Def;

	//(kg * cm/(s^2))/cm
	UPROPERTY(VisibleAnywhere)
	float VirticalSuspension;

	//(kg * cm/(s^2))/cm
	UPROPERTY(VisibleAnywhere)
	float HorizontalSuspension;

	UPROPERTY(VisibleAnywhere)
	EPedalState MovementState;

	UPROPERTY(VisibleAnywhere)
	bool bOnWall;

	UPROPERTY(VisibleAnywhere)
	FVector StartLocation;

	UPROPERTY(VisibleAnywhere)
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

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	FTransform MeshRelativeTransform;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	FVector BoxCollisionExtent;

	// cm/(s^2)  110 = 8초에 시속 32km/h 수준
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float Acceleration = 110.0f;

#if WITH_EDITORONLY_DATA
	UFUNCTION(CallInEditor, Category = "Util")
	void MeasurePedalProperties();

	FVector GetBoneDefaultLocation(USkeletalMesh* SkeletalMesh, FName BoneName);
#endif
};
