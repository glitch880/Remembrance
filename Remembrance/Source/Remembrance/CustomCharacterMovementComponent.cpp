// Fill out your copyright notice in the Description page of Project Settings.

#include "Remembrance.h"
#include "CustomCharacterMovementComponent.h"
#include "Engine.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent()
{
	SetWalkableFloorZ(0.71f);
}

void UCustomCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case (uint8)ECustomMovementMode::Gliding:
		PhysCustomGliding(DeltaTime, Iterations);
		break;
	case (uint8)ECustomMovementMode::None:
	default:
		// Call blueprint custom movement event
		Super::PhysCustom(DeltaTime, Iterations);
		break;
	}

}
const float VERTICAL_SLOPE_NORMAL_Z = 0.001f;
void UCustomCharacterMovementComponent::PhysCustomGliding(float DeltaTime, int32 Iterations)
{
	//

	//FVector FallAcceleration = GetFallingLateralAcceleration(DeltaTime);
	//FallAcceleration.Z = 0.f;
	//const bool bHasAirControl = (FallAcceleration.SizeSquared2D() > 0.f);

	//float remainingTime = DeltaTime;
	//while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations))
	//{
	//	Iterations++;
	//	const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
	//	remainingTime -= timeTick;

	//	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	//	const FQuat PawnRotation = UpdatedComponent->GetComponentQuat();
	//	bJustTeleported = false;

	//	FVector OldVelocity = Velocity;
	//	FVector VelocityNoAirControl = Velocity;

	//	// Apply input
	//	if (!HasRootMotion())
	//	{
	//		// Compute VelocityNoAirControl
	//		if (bHasAirControl)
	//		{
	//			// Find velocity *without* acceleration.
	//			TGuardValue<FVector> RestoreAcceleration(Acceleration, FVector::ZeroVector);
	//			TGuardValue<FVector> RestoreVelocity(Velocity, Velocity);
	//			Velocity.Z = 0.f;
	//			CalcVelocity(timeTick, FallingLateralFriction, false, BrakingDecelerationFalling);
	//			VelocityNoAirControl = FVector(Velocity.X, Velocity.Y, OldVelocity.Z);
	//		}

	//		// Compute Velocity
	//		{
	//			// Acceleration = FallAcceleration for CalcVelocity(), but we restore it after using it.
	//			TGuardValue<FVector> RestoreAcceleration(Acceleration, FallAcceleration);
	//			Velocity.Z = 0.f;
	//			CalcVelocity(timeTick, FallingLateralFriction, false, BrakingDecelerationFalling);
	//			Velocity.Z = OldVelocity.Z;
	//		}

	//		// Just copy Velocity to VelocityNoAirControl if they are the same (ie no acceleration).
	//		if (!bHasAirControl)
	//		{
	//			VelocityNoAirControl = Velocity;
	//		}
	//	}

	//	// Apply gravity
	//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Aliens, %f, %f"), Velocity.Z, OldVelocity.Z));

	//	const FVector Gravity(0.f, 0.f, GetGravityZ());
	//	Velocity = NewFallVelocity(Velocity, Gravity, timeTick);
	//	VelocityNoAirControl = NewFallVelocity(VelocityNoAirControl, Gravity, timeTick);
	//	const FVector AirControlAccel = (Velocity - VelocityNoAirControl) / timeTick;
	//	
	//	Velocity.Z = FMath::Clamp(Velocity.Z, -100.f, 0.f);
	//	
	//	//DEBUG TODO
	//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Aliens, %f"), Gravity.Z));

	//	if (bNotifyApex && CharacterOwner->Controller && (Velocity.Z <= 0.f))
	//	{
	//		// Just passed jump apex since now going down
	//		bNotifyApex = false;
	//		NotifyJumpApex();
	//	}


	//	// Move
	//	FHitResult Hit(1.f);
	//	FVector Adjusted = 0.5f*(OldVelocity + Velocity) * timeTick;
	//	SafeMoveUpdatedComponent(Adjusted, PawnRotation, true, Hit);

	//	if (!HasValidData())
	//	{
	//		return;
	//	}

	//	float LastMoveTimeSlice = timeTick;
	//	float subTimeTickRemaining = timeTick * (1.f - Hit.Time);

	//	if (IsSwimming()) //just entered water
	//	{
	//		remainingTime += subTimeTickRemaining;
	//		StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
	//		return;
	//	}
	//	else if (Hit.bBlockingHit)
	//	{
	//		if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
	//		{
	//			remainingTime += subTimeTickRemaining;
	//			ProcessLanded(Hit, remainingTime, Iterations);
	//			return;
	//		}
	//		else
	//		{
	//			// Compute impact deflection based on final velocity, not integration step.
	//			// This allows us to compute a new velocity from the deflected vector, and ensures the full gravity effect is included in the slide result.
	//			Adjusted = Velocity * timeTick;

	//			// See if we can convert a normally invalid landing spot (based on the hit result) to a usable one.
	//			if (!Hit.bStartPenetrating && ShouldCheckForValidLandingSpot(timeTick, Adjusted, Hit))
	//			{
	//				const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
	//				FFindFloorResult FloorResult;
	//				FindFloor(PawnLocation, FloorResult, false);
	//				if (FloorResult.IsWalkableFloor() && IsValidLandingSpot(PawnLocation, FloorResult.HitResult))
	//				{
	//					remainingTime += subTimeTickRemaining;
	//					ProcessLanded(FloorResult.HitResult, remainingTime, Iterations);
	//					return;
	//				}
	//			}

	//			HandleImpact(Hit, LastMoveTimeSlice, Adjusted);

	//			// If we've changed physics mode, abort.
	//			if (!HasValidData() || !IsFalling())
	//			{
	//				return;
	//			}

	//			// Limit air control based on what we hit.
	//			// We moved to the impact point using air control, but may want to deflect from there based on a limited air control acceleration.
	//			if (bHasAirControl)
	//			{
	//				const bool bCheckLandingSpot = false; // we already checked above.
	//				const FVector AirControlDeltaV = LimitAirControl(LastMoveTimeSlice, AirControlAccel, Hit, bCheckLandingSpot) * LastMoveTimeSlice;
	//				Adjusted = (VelocityNoAirControl + AirControlDeltaV) * LastMoveTimeSlice;
	//			}

	//			const FVector OldHitNormal = Hit.Normal;
	//			const FVector OldHitImpactNormal = Hit.ImpactNormal;
	//			FVector Delta = ComputeSlideVector(Adjusted, 1.f - Hit.Time, OldHitNormal, Hit);

	//			// Compute velocity after deflection (only gravity component for RootMotion)
	//			if (subTimeTickRemaining > KINDA_SMALL_NUMBER && !bJustTeleported)
	//			{
	//				const FVector NewVelocity = (Delta / subTimeTickRemaining);
	//				Velocity = HasRootMotion() ? FVector(Velocity.X, Velocity.Y, NewVelocity.Z) : NewVelocity;
	//			}

	//			if (subTimeTickRemaining > KINDA_SMALL_NUMBER && (Delta | Adjusted) > 0.f)
	//			{
	//				// Move in deflected direction.
	//				SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);

	//				if (Hit.bBlockingHit)
	//				{
	//					// hit second wall
	//					LastMoveTimeSlice = subTimeTickRemaining;
	//					subTimeTickRemaining = subTimeTickRemaining * (1.f - Hit.Time);

	//					if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
	//					{
	//						remainingTime += subTimeTickRemaining;
	//						ProcessLanded(Hit, remainingTime, Iterations);
	//						return;
	//					}

	//					HandleImpact(Hit, LastMoveTimeSlice, Delta);

	//					// If we've changed physics mode, abort.
	//					if (!HasValidData() || !IsFalling())
	//					{
	//						return;
	//					}

	//					// Act as if there was no air control on the last move when computing new deflection.
	//					if (bHasAirControl && Hit.Normal.Z > VERTICAL_SLOPE_NORMAL_Z)
	//					{
	//						const FVector LastMoveNoAirControl = VelocityNoAirControl * LastMoveTimeSlice;
	//						Delta = ComputeSlideVector(LastMoveNoAirControl, 1.f, OldHitNormal, Hit);
	//					}

	//					FVector PreTwoWallDelta = Delta;
	//					TwoWallAdjust(Delta, Hit, OldHitNormal);

	//					// Limit air control, but allow a slide along the second wall.
	//					if (bHasAirControl)
	//					{
	//						const bool bCheckLandingSpot = false; // we already checked above.
	//						const FVector AirControlDeltaV = LimitAirControl(subTimeTickRemaining, AirControlAccel, Hit, bCheckLandingSpot) * subTimeTickRemaining;

	//						// Only allow if not back in to first wall
	//						if (FVector::DotProduct(AirControlDeltaV, OldHitNormal) > 0.f)
	//						{
	//							Delta += (AirControlDeltaV * subTimeTickRemaining);
	//						}
	//					}

	//					// Compute velocity after deflection (only gravity component for RootMotion)
	//					if (subTimeTickRemaining > KINDA_SMALL_NUMBER && !bJustTeleported)
	//					{
	//						const FVector NewVelocity = (Delta / subTimeTickRemaining);
	//						Velocity = HasRootMotion() ? FVector(Velocity.X, Velocity.Y, NewVelocity.Z) : NewVelocity;
	//					}

	//					// bDitch=true means that pawn is straddling two slopes, neither of which he can stand on
	//					bool bDitch = ((OldHitImpactNormal.Z > 0.f) && (Hit.ImpactNormal.Z > 0.f) && (FMath::Abs(Delta.Z) <= KINDA_SMALL_NUMBER) && ((Hit.ImpactNormal | OldHitImpactNormal) < 0.f));
	//					SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);
	//					if (Hit.Time == 0.f)
	//					{
	//						// if we are stuck then try to side step
	//						FVector SideDelta = (OldHitNormal + Hit.ImpactNormal).GetSafeNormal2D();
	//						if (SideDelta.IsNearlyZero())
	//						{
	//							SideDelta = FVector(OldHitNormal.Y, -OldHitNormal.X, 0).GetSafeNormal();
	//						}
	//						SafeMoveUpdatedComponent(SideDelta, PawnRotation, true, Hit);
	//					}

	//					if (bDitch || IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit) || Hit.Time == 0.f)
	//					{
	//						remainingTime = 0.f;
	//						ProcessLanded(Hit, remainingTime, Iterations);
	//						return;
	//					}
	//					else if (GetPerchRadiusThreshold() > 0.f && Hit.Time == 1.f && OldHitImpactNormal.Z >= WalkableFloorZ)
	//					{
	//						// We might be in a virtual 'ditch' within our perch radius. This is rare.
	//						const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
	//						const float ZMovedDist = FMath::Abs(PawnLocation.Z - OldLocation.Z);
	//						const float MovedDist2DSq = (PawnLocation - OldLocation).SizeSquared2D();
	//						if (ZMovedDist <= 0.2f * timeTick && MovedDist2DSq <= 4.f * timeTick)
	//						{
	//							Velocity.X += 0.25f * GetMaxSpeed() * (FMath::FRand() - 0.5f);
	//							Velocity.Y += 0.25f * GetMaxSpeed() * (FMath::FRand() - 0.5f);
	//							Velocity.Z = FMath::Max<float>(JumpZVelocity * 0.25f, 1.f);
	//							Delta = Velocity * timeTick;
	//							SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}

	//	if (Velocity.SizeSquared2D() <= KINDA_SMALL_NUMBER * 10.f)
	//	{
	//		Velocity.X = 0.f;
	//		Velocity.Y = 0.f;
	//	}
	//}
	//

}