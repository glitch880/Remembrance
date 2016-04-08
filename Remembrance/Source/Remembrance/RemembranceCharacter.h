// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "FlyingComp.h"
#include "RemembranceCharacter.generated.h"

UCLASS(config=Game)
class ARemembranceCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UFlyingComp* FlyingComponent;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	ARemembranceCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Jumping, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float fJumpingTurnRate;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Jumping)
		float StandardYawRotation;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Flying, meta = (ClampMin = "0.0", ClampMax = "10.0", UIMin = "0.0", UIMax = "10.0"))
	float fFallTimeBeforeFlying;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Flying, meta = (ClampMin = "0.0", ClampMax = "10.0", UIMin = "0.0", UIMax = "10.0"))
	float GroundTimeBeforeShapeshift;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Flying, meta = (ClampMin = "-10000.0", ClampMax = "0.0", UIMin = "-10000.0", UIMax = "0.0"))
		float FallingLineTraceLenght;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Flying, meta = (ClampMin = "-10000.0", ClampMax = "0.0", UIMin = "-10000.0", UIMax = "0.0"))
		float FlyingLineTraceLenght;

	UPROPERTY(BlueprintReadWrite, Category = Flying)
		bool bCanFly;

	UPROPERTY(BlueprintReadWrite, Category = Flying)
		bool bCanSwitchToWalking;

	//how long before transformation triggers / How long untill player runs out of air
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Swimming)
		float fBreathTime;
	//are we under water or not
	UPROPERTY(BlueprintReadWrite, Category = Swimming)
		bool bSubmerged;

	//Speed in water form
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Swimming)
		float fTransformedSwimSpeed;


	UPROPERTY(EditAnywhere, category="Cameras")
		AActor* ThirdPersonCamera;
	UPROPERTY(EditAnywhere, category="Cameras")
		AActor* FirstPersonCamera;


	float fSwimHeightVector;

protected:
	//Called for actions using the "jump" key
	void CustomJump();
	void CustomStopJump();

	//Called for actions using the "Crouch" key
	void CustomCrouch();
	void CustomStopCrouch();


	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/** Tick Function for the character **/
	virtual void Tick(float DeltaSeconds) override;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	/** This happens when switching from one Movement_type to another manually */
	void SwitchMovementType(EMovementMode mode);

	/** Function that checks if we should start flying */
	void CheckIfWeShouldFly(float DeltaSeconds);

	/** Function that checks if we should stop flying */
	void CheckIfWeShouldStopFlying(float DeltaSeconds);

private:

	//how long the character has been submerged
	float fcurrentTimeSubmerged;

	/** How long the character has currently fallen.*/
	float fCurrentFallTime;

	/** How long the character has currently fallen.*/
	float fCurrentGroundTime;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintNativeEvent, Category = "Flying")
	void OnTimeTriggered();

	UFUNCTION(BlueprintNativeEvent, Category = "Flying")
	void OnFlyingToWalkingTrigger();

	UFUNCTION(BlueprintNativeEvent, Category = "Swimming")
		void CheckSubmerged();

	UFUNCTION(BlueprintNativeEvent, Category = "Flying")
		void FlyingCollisionTrigger();

	UFUNCTION(BlueprintCallable, Category = "Flying")
		void SetLengthToObject(float Lenght);

	UFUNCTION(BlueprintCallable, Category = "Flying")
		void SetCollisionDetected(bool Collision);

	UFUNCTION(BlueprintCallable, Category = "Flying")
		void SetPushValue(float value);
	

};

