// Fill out your copyright notice in the Description page of Project Settings.

#include "Remembrance.h"
#include "Engine.h"
#include "FlyingComp.h"


// Sets default values for this component's properties
UFlyingComp::UFlyingComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = false;
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	TurnSpeed = 50.f;
	MaxSpeed = 400.f;
	MinSpeed = 500.f;
	CurrentForwardSpeed = 0.f;
	RollScale = 1.0f;
	YawScale = 1.0f;
	PitchScale = 1.0f;
	PitchVelocityGain = 50.f;
	PitchVelocityLoss = 50.f;
	PitchForce = 100.f;
	SpeedLossEachSecond = 10.f;
	MinPitchdownForce = 5.f;
	MaxPitchdownForce = 20.f;
	NormalizeRollScale = 3.f;
	MaxPitchUpAngle = 15.f;
	MaxRollAngle = 50.f;
	bIsTurning = false;
	RollHardness = 3.f;
	PitchHardness = 3.f;
	YawHardness = 3.f;
	MaxPitchStrenght = 50.f;
	MaxRollStrenght = 50.f;
	MaxYawStrenght = 50.f;
	TraceLenght = 3000.f;
}


// Called when the game starts
void UFlyingComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

//Called every frame
void UFlyingComp::MyOwnTick(float DeltaTime)
{
	WallRayCasting();

	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaTime, 0.f, 0.f);

//	FlyingCollisionTrigger();
	if (GetOwner()->GetActorForwardVector().Z < 0)
		CurrentForwardSpeed -= GetOwner()->GetActorForwardVector().Z * DeltaTime * PitchVelocityGain;
	else if (GetOwner()->GetActorForwardVector().Z > 0)
		CurrentForwardSpeed -= GetOwner()->GetActorForwardVector().Z * DeltaTime * PitchVelocityLoss;

	CurrentForwardSpeed -= DeltaTime * SpeedLossEachSecond;

	CurrentForwardSpeed = FMath::Clamp(CurrentForwardSpeed, MinSpeed, MaxSpeed);

	//UE_LOG(LogTemp, Warning, TEXT("pusvalue: %f, %f"), SmoothStep(2, -1.f, 1.f), PushValue);


	/*if (CurrentForwardSpeed < 0)
	CurrentForwardSpeed = CurrentForwardSpeed / 2;*/

	// Move plan forwards (with sweep so we stop when we collide with things)
	GetOwner()->AddActorLocalOffset(LocalMove, true);

	//UE_LOG(LogTemp, Warning, TEXT("Speed: %f"), CurrentForwardSpeed);


	// Calculate change in rotation this frame
	FRotator DeltaRotation(0, 0, 0);
	if (bIsTurning || bCollisionDetected)
		DeltaRotation.Pitch = CurrentPitchTurnSpeed * DeltaTime;
	else
		DeltaRotation.Pitch = CurrentPitchSpeed * DeltaTime;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaTime;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaTime;

	// Rotate plane
	GetOwner()->AddActorLocalRotation(DeltaRotation);
}

void UFlyingComp::WallRayCasting()
{
	FHitResult* HitResult = new FHitResult();
	FVector StartTrace = GetOwner()->GetActorLocation();
	FVector EndTrace = StartTrace + (GetOwner()->GetActorForwardVector() * TraceLenght);
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace, ECC_Visibility, *TraceParams))
	{
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0,0), true);

		bCollisionDetected = HitResult->bBlockingHit;
		LenghtToObject = (HitResult->ImpactPoint - GetOwner()->GetActorLocation()).Size();
		PushValue = FVector::CrossProduct(HitResult->ImpactNormal, GetOwner()->GetActorForwardVector()).Z;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Value: %f"), PushValue));
	}
	else
		bCollisionDetected = false;

}

// Called every frame
void UFlyingComp::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void UFlyingComp::MoveUpInput(float Val)
{
	float TargetPitchSpeed = 0.0f;
	// Target pitch speed is based in input


	//if(CurrentForwardSpeed > MaxSpeed / 3)
	TargetPitchSpeed = (Val * TurnSpeed * -1.f * PitchScale * FMath::Clamp((MaxPitchUpAngle + GetOwner()->GetActorRotation().Pitch * Val), 0.f, 1.f));

	float removespeed = FMath::Clamp(CurrentForwardSpeed, 1.f, MaxSpeed);
	float downspeed = (MaxSpeed - removespeed) / PitchForce;
	downspeed = FMath::Clamp(downspeed, MinPitchdownForce, MaxPitchdownForce);

	if (!bIsTurning)
		TargetPitchSpeed -= downspeed;

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), PitchHardness);
}

void UFlyingComp::MoveRightInput(float Val)
{
	
	float TargetRollSpeed = 0.0f;
	float TargetPitchSpeed = 0.0f;
	float TargetYawSpeed = 0.0f;
	if (!bCollisionDetected || FMath::Abs(PushValue) < 0.001)
	{
		if (Val != 0 && !bIsTurning)
		{
			CurrentPitchTurnSpeed = FMath::FInterpTo(CurrentPitchSpeed, CurrentPitchTurnSpeed, GetWorld()->GetDeltaSeconds(), 10.f);
			bIsTurning = true;
		}
		else if (Val == 0 && bIsTurning)
		{
			CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchTurnSpeed, CurrentPitchSpeed, GetWorld()->GetDeltaSeconds(), 10.f);
			bIsTurning = false;
		}

		TargetRollSpeed = bIsTurning ? (Val * TurnSpeed * RollScale * FMath::Clamp((MaxRollAngle - GetOwner()->GetActorRotation().Roll * Val), 0.f, 1.f)) : (GetOwner()->GetActorRotation().Roll * -NormalizeRollScale);
		TargetPitchSpeed = (TurnSpeed * PitchScale * FMath::Clamp(((GetOwner()->GetActorRotation().Roll * Val) / MaxRollAngle), 0.f, 1.f) * FMath::Clamp((30 + GetOwner()->GetActorRotation().Pitch * Val), 0.f, 1.f));
		TargetYawSpeed = (Val * TurnSpeed * YawScale * FMath::Clamp((MaxRollAngle / (GetOwner()->GetActorRotation().Roll * Val)), 0.f, 1.f));
	}
	else
	{
		float Pushing = 0.f;

		if (PushValue < -0.001f)
			Pushing = 1.f;
		else if (PushValue > 0.001f)
			Pushing = -1.f;

		TargetRollSpeed = FMath::Clamp(((Pushing * TurnSpeed * RollScale * FMath::Clamp((MaxRollAngle - GetOwner()->GetActorRotation().Roll * Pushing), 0.f, 1.f)) * WallCollisionPushStrenght / LenghtToObject), -MaxRollStrenght, MaxRollStrenght);
		TargetPitchSpeed = FMath::Clamp(((TurnSpeed * PitchScale * FMath::Clamp(((GetOwner()->GetActorRotation().Roll * Pushing) / MaxRollAngle), 0.f, 1.f) * FMath::Clamp((30 + GetOwner()->GetActorRotation().Pitch * Pushing), 0.f, 1.f)) * WallCollisionPushStrenght / LenghtToObject), -MaxPitchStrenght, MaxPitchStrenght);
		TargetYawSpeed = FMath::Clamp(((Pushing * TurnSpeed * YawScale * FMath::Clamp((MaxRollAngle / (GetOwner()->GetActorRotation().Roll * Pushing)), 0.f, 1.f)) * WallCollisionPushStrenght / LenghtToObject), -MaxYawStrenght, MaxYawStrenght);


		UE_LOG(LogTemp, Warning, TEXT("Y, P, R: %f"), WallCollisionPushStrenght / LenghtToObject);
	}



	//// Smoothly interpolate to target yaw speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), RollHardness);
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), YawHardness);
	CurrentPitchTurnSpeed = FMath::FInterpTo(CurrentPitchTurnSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), PitchHardness);
}

