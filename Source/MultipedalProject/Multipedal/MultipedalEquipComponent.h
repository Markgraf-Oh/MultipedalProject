// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/StreamableManager.h"
#include "MultipedalEquipComponent.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPEDALPROJECT_API UMultipedalEquipComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UMultipedalEquipComponent();

	virtual void BeginPlay() override;


private:
	UPROPERTY(EditDefaultsOnly)
	FName MultipedalBodyId;

	TSharedPtr<FStreamableHandle> BodyAssetLoadHandle;

	UPROPERTY(Transient)
	class UMultipedalDataAsset* MultipedalDataAsset;

	void PostBodyAssetLoad();
};