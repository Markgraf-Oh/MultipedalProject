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

	//load�ϱ� ���� ���� ID, ItemID�� ����.
	UPROPERTY(EditAnywhere, Category = "PrimaryDataAsset")
	FName ItemID = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category = "Asset", meta = (AssetBundles = "Visual"))
	TSoftObjectPtr<class USkeletalMesh> Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Asset", meta = (AssetBundles = "Animation"))
	TSoftClassPtr<class UAnimInstance> Anim;

	UPROPERTY(EditDefaultsOnly, Category = "Pedal")
	TArray<FPedalDefinition> PedalDefList;
};
