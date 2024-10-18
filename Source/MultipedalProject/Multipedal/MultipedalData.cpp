// Fill out your copyright notice in the Description page of Project Settings.


#include "MultipedalData.h"
#include "Curves/CurveFloat.h"
#include "Engine/AssetManager.h"

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
