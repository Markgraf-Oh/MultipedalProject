// Fill out your copyright notice in the Description page of Project Settings.


#include "MultipedalMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/EngineTypes.h"

//바디의 중심이 아닌 곳에 힘이 가해질 때, 힘 중 얼마만큼이 토크로 전환되는 지 여부
static float TorqueTransferFactor = 0.1f;
static FAutoConsoleVariableRef CVarTorqueTransferFactor(
	TEXT("dev.Physics.TorqueTransferFactor"),
	TorqueTransferFactor,
	TEXT(""),
	ECVF_Cheat);

UMultipedalMovementComponent::UMultipedalMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMultipedalMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(UpdatedPrimitive) && UpdatedPrimitive->GetBodyInstance())
	{
		if (bLocalControl.IsSet() && bLocalControl.GetValue())
		{
			UpdatedPrimitive->GetBodyInstance()->AddCustomPhysics(CalculateCustomPhysics);
		}
	}
}

void UMultipedalMovementComponent::InitializeMultipedalData(class UMultipedalDataAsset* MultipedalData)
{
	if (!IsValid(MultipedalData) || !IsValid(UpdatedPrimitive) || !UpdatedPrimitive->GetBodyInstance())
	{
		return;
	}

	CalculateCustomPhysics.BindUObject(this, &ThisClass::CustomPhysicsSubstepTick);

	BodyTransform = UpdatedPrimitive->GetBodyInstance()->GetUnrealWorldTransform();

	PedalList.Empty();
	PedalList.SetNum(MultipedalData->PedalDefList.Num());

	GroundCollisionQueryParams = FCollisionQueryParams();
	GroundCollisionQueryParams.bTraceComplex = true;
	GroundCollisionQueryParams.bReturnFaceIndex = false;
	GroundCollisionQueryParams.bReturnPhysicalMaterial = false;
	GroundCollisionQueryParams.AddIgnoredActor(GetOwner());

	for (int32 PedalIndex = 0; PedalIndex < PedalList.Num(); PedalIndex++)
	{
		FPedal& CurrentPedal = PedalList[PedalIndex];
		CurrentPedal.Def = MultipedalData->PedalDefList[PedalIndex];
		//Relative->World
		FVector InitialFootWorldLoc = UKismetMathLibrary::TransformLocation(BodyTransform, CurrentPedal.Def.InitialRelativeLocation);
		
		FVector Upward = FVector::UpVector;
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, InitialFootWorldLoc + Upward * CurrentPedal.Def.UpwardRange, InitialFootWorldLoc - Upward * CurrentPedal.Def.DownwardRange, ECollisionChannel::ECC_PhysicsBody, GroundCollisionQueryParams);
		
		if (Hit.bBlockingHit)
		{
			CurrentPedal.MovementState = EPedalState::OnSurface;
			CurrentPedal.StartLocation = Hit.Location;
			CurrentPedal.TargetLocation = Hit.Location;
		}
		else
		{
			CurrentPedal.MovementState = EPedalState::Hanging;
			CurrentPedal.StartLocation = InitialFootWorldLoc;
			CurrentPedal.TargetLocation = InitialFootWorldLoc;
		}
	}
	
	if (!ensure(IsValid(UpdatedPrimitive) && UpdatedPrimitive->GetBodyInstance()))
	{
		return;
	}

	FBodyInstance* BodyInstance = UpdatedPrimitive->GetBodyInstance();
	TotalMassKg = BodyInstance->GetBodyMass();

	// kg * cm / (s^2)
	FVector TotalGravityForce = GetGravityVector() * TotalMassKg;

	
	COMOffset = BodyInstance->GetMassSpaceLocal().GetLocation();

	//다리별 지탱해야하는 힘계산
	//좌우 대칭임을 가정
	ensure((PedalList.Num() % 2) == 0);
	int32 NumPedalEachSide = PedalList.Num() / 2;

	//각 다리별로 균등하게 힘을 분배했을 때 발생하는 여분의 토크를 계산
	//오른쪽 다리들의 토크 합
	FVector TorqueSum = FVector::ZeroVector;
	FVector LiftForceEach =  - (1.0f / float(PedalList.Num())) * TotalGravityForce;
	for (int32 RightSideIndex = 0; RightSideIndex < NumPedalEachSide; RightSideIndex++)
	{
		FVector ForwardDistance = (PedalList[RightSideIndex].Def.RootRelativeLocation.X - COMOffset.X) * FVector::ForwardVector;
		TorqueSum = TorqueSum + FVector::CrossProduct(ForwardDistance, LiftForceEach);
	}

	//Roll방향 토크는 좌우대칭으로 상쇄. Yaw방향 토크는 중력이 Z축일 때 존재하지 않음.
	//힘을 균등분배했을 때의 토크를 상쇄할 수 있도록 각 다리에 분배하여 힘을 적절히 증감하도록 한다.
	float TorquePitchAdjustEach = -TorqueSum.Y / NumPedalEachSide;

	for (FPedal& Pedal : PedalList)
	{
		float DistanceX = Pedal.Def.RootRelativeLocation.X - COMOffset.X;

		float PedalForce = LiftForceEach.Z + TorquePitchAdjustEach / DistanceX;

		//다리의 지지력과 다리가 늘어나는 최대거리를 기반으로 다리에 의한 서스펜션 작용을 역산
		Pedal.VirticalSuspension = PedalForce / Pedal.Def.DownwardRange;
	}
}

void UMultipedalMovementComponent::ActivateLocalControl(bool bInActivate)
{
	if (!IsValid(UpdatedPrimitive) || !UpdatedPrimitive->GetBodyInstance())
	{
		return;
	}

	if (!bLocalControl.IsSet() || bLocalControl.GetValue() != bInActivate)
	{
		bLocalControl = bInActivate;
		SetComponentTickEnabled(bLocalControl.GetValue());

		if (bLocalControl.GetValue())
		{

		}
	}
}

void UMultipedalMovementComponent::CustomPhysicsSubstepTick(float DeltaTime, FBodyInstance* BodyInstance)
{
	DrawDebugBox(GetWorld(), BodyInstance->GetCOMPosition(), FVector(10.0f), FQuat::Identity, FColor::Red);
	DrawDebugBox(GetWorld(), BodyInstance->GetUnrealWorldTransform().GetLocation(), FVector(10.0f), FQuat::Identity, FColor::Blue);

	//Calc Step
	//Progress Step

	BeginForceCalc(DeltaTime, BodyInstance);
	CalcVerticalSuspentionForce(DeltaTime, BodyInstance);
	ApplyForceCalc(DeltaTime, BodyInstance);
}

void UMultipedalMovementComponent::BeginForceCalc(float DeltaTime, FBodyInstance* BodyInstance)
{
	TotalForce = FVector::ZeroVector;
	TotalTorque = FVector::ZeroVector;
	LinearVelocity = BodyInstance->GetUnrealWorldVelocity();
	AngularVelocity = BodyInstance->GetUnrealWorldAngularVelocityInRadians();
	BodyTransform = BodyInstance->GetUnrealWorldTransform();
	COMWorldLocation = BodyInstance->GetCOMPosition();
}

void UMultipedalMovementComponent::CalcVerticalSuspentionForce(float DeltaTime, FBodyInstance* BodyInstance)
{
	//TArray<FVector> PedalLiftForce;
	for (FPedal& Pedal : PedalList)
	{
		if (Pedal.MovementState != EPedalState::OnSurface)
		{
			continue;
		}
		//Relative->World
		FVector ForceApplyLoc = UKismetMathLibrary::TransformLocation(BodyTransform, Pedal.Def.RootRelativeLocation);
		float MaxExtend = (COMOffset - Pedal.Def.InitialRelativeLocation).Z + Pedal.Def.DownwardRange;
		float ZOffset = FVector::DotProduct(ForceApplyLoc - Pedal.TargetLocation, FVector::UpVector);
		if (ZOffset < MaxExtend - (Pedal.Def.DownwardRange + Pedal.Def.UpwardRange))
		{
			//다리가 동작범위를 벗어남
		}
		float CurDistance = MaxExtend - ZOffset;
		FVector LiftForce = FVector::UpVector * (CurDistance * Pedal.VirticalSuspension);
		TotalForce += LiftForce;
		TotalTorque += FVector::CrossProduct(ForceApplyLoc - COMWorldLocation, LiftForce) * TorqueTransferFactor;

		DrawDebugLine(GetWorld(), Pedal.TargetLocation, Pedal.TargetLocation + LiftForce * 0.005f, FColor::Green);
	}
}

void UMultipedalMovementComponent::ApplyForceCalc(float DeltaTime, FBodyInstance* BodyInstance)
{
	BodyInstance->AddForce(TotalForce, false, false);
	BodyInstance->AddTorqueInRadians(TotalTorque, false, false);
}
