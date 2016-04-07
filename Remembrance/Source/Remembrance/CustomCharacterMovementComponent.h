// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class REMEMBRANCE_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	
public:
	UCustomCharacterMovementComponent();	

protected:
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

private:
	// Custom movement code for surf movement
	void PhysCustomGliding(float DeltaTime, int32 Iterations);

	// Custom movement modes
	enum class ECustomMovementMode : uint8
	{
		None,
		Gliding
	};
	
	float WalkableFloorZ;

};
