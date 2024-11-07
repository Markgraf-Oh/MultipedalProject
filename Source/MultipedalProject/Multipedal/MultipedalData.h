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

	//BodyInstance�� ���� �����ġ
	UPROPERTY(EditDefaultsOnly)
	FVector InitialRelativeLocation;

	//BodyInstance�� ���� �����ġ
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
	//��ǥ�鿡 ������ ����
	OnSurface,
	//�̵��� ���� �ǵ������� ���߿��� �����̰� �ִ� ����
	Stepping,
	//�̵� ��ǥ������ ��ȿȭ �Ǿ��ų�, �ܺο����������� �����κ��� �־��� ���߿� ���ִ� ����
	Hanging
};

//�� �ٸ��� Instance
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

	//load�ϱ� ���� ���� ID, ItemID�� ����.
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

	// cm/(s^2)  110 = 8�ʿ� �ü� 32km/h ����
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float Acceleration = 110.0f;

#if WITH_EDITORONLY_DATA
	UFUNCTION(CallInEditor, Category = "Util")
	void MeasurePedalProperties();

	FVector GetBoneDefaultLocation(USkeletalMesh* SkeletalMesh, FName BoneName);
#endif
};
