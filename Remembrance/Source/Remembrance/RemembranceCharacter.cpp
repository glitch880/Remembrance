// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Remembrance.h"
#include "RemembranceCharacter.h"

//////////////////////////////////////////////////////////////////////////
// ARemembranceCharacter

ARemembranceCharacter::ARemembranceCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	fJumpingTurnRate = 0.5f;
	fFallTimeBeforeFlying = 2.0f;
	fCurrentFallTime = 0.0f;
	GroundTimeBeforeShapeshift = 2.0f;
	StandardYawRotation = 540.f;


	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bCanFly = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


	//Swimming setup
	fSwimHeightVector = 0.0f;
	fTransformedSwimSpeed = 600.f;
	bSubmerged = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARemembranceCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ARemembranceCharacter::CustomJump);
	InputComponent->BindAction("Jump", IE_Released, this, &ARemembranceCharacter::CustomStopJump);

	InputComponent->BindAction("Crouch", IE_Pressed, this, &ARemembranceCharacter::CustomCrouch);
	InputComponent->BindAction("Crouch", IE_Released, this, &ARemembranceCharacter::CustomStopCrouch);

	InputComponent->BindAxis("MoveForward", this, &ARemembranceCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ARemembranceCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ARemembranceCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ARemembranceCharacter::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &ARemembranceCharacter::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &ARemembranceCharacter::TouchStopped);
}

void ARemembranceCharacter::SwitchMovementType(EMovementMode mode)
{
	GetCharacterMovement()->SetMovementMode(mode);

	switch (GetCharacterMovement()->MovementMode)
	{
		case MOVE_None:
			break;
		case MOVE_Walking:
			fCurrentFallTime = 0.0f;
			break;
		case MOVE_Falling:
		{
			break;
		}
		case MOVE_Swimming:
		{
			break;
		}
		case MOVE_Flying:
		{
			fCurrentFallTime = 0.0f;

			break;
		}

	}

}


void ARemembranceCharacter::Tick(float DeltaSeconds)
{
	switch (GetCharacterMovement()->MovementMode)
	{
	case MOVE_None:
		break;
	case MOVE_Walking:
		break;
	case MOVE_Falling:
	{
		//If we are falling then we should check if we can start flying instead.
		CheckIfWeShouldFly(DeltaSeconds);

		break;
	}
	case MOVE_Swimming:
	{
		//TODO increase time below surface if character  almost fully submerged. Potential solution : use line tract straight up and compare distance with a threshold.
		CheckSubmerged();
		if(bSubmerged ) //change to condition
		fcurrentTimeSubmerged += 1.0f * DeltaSeconds;

		//enable swimming up
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation(0, Rotation.Yaw, 0);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Z);
		AddMovementInput(Direction, fSwimHeightVector);
		break;
	}
	case MOVE_Flying:

		//If we are flying, we should constantly check if we should continue or stop.
		CheckIfWeShouldStopFlying(DeltaSeconds);

		break;
	}
}

void ARemembranceCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void ARemembranceCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void ARemembranceCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARemembranceCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARemembranceCharacter::CheckSubmerged_Implementation()
{
	// Logic needed when blueprints don't implement the event. Can be empty.
}

void ARemembranceCharacter::OnTimeTriggered_Implementation()
{
	// Logic needed when blueprints don't implement the event. Can be empty.
}

void ARemembranceCharacter::OnFlyingToWalkingTrigger_Implementation()
{
	// Logic needed when blueprints don't implement the event. Can be empty.
}

void ARemembranceCharacter::CustomJump()
{
	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	switch (GetCharacterMovement()->MovementMode)
	{
	case MOVE_None:
		break;
	case MOVE_Walking:
		ACharacter::Jump();
		break;
	case MOVE_Falling:
		
		break;
	case MOVE_Swimming:
		//enable swimming up
		UE_LOG(LogTemp, Warning, TEXT("Swimming up"));
		fSwimHeightVector = 1.0f;
		break;
	case MOVE_Flying:

		
		break;
	}
}

void ARemembranceCharacter::CustomStopJump()
{
	fSwimHeightVector = 0.0f;
	switch (GetCharacterMovement()->MovementMode)
	{
	case MOVE_None:
		break;
	case MOVE_Walking:
		ACharacter::StopJumping();

		break;
	case MOVE_Falling:


		break;
	case MOVE_Swimming:
		//stop swimming
		fSwimHeightVector = 0.0f;
		break;
	case MOVE_Flying:


		break;
	}
}

void ARemembranceCharacter::CustomCrouch()
{
	switch (GetCharacterMovement()->MovementMode)
	{
	case MOVE_None:
		break;
	case MOVE_Walking:
	{
	/*	UE_LOG(LogTemp, Warning, TEXT("Camera plz"));
		APlayerController* tempControll = GetWorld()->GetFirstPlayerController();

		tempControll->SetViewTargetWithBlend(FirstPerson, 5.f);

		bUseControllerRotationPitch = true;
		bUseControllerRotationYaw = true;
		bUseControllerRotationRoll = true; */

		break;
	}
	case MOVE_Falling: 
	{
	/*	APlayerController* tempControll = GetWorld()->GetFirstPlayerController();

		tempControll->SetViewTargetWithBlend(ThirdPerson, 5.f);

		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false; */

		break;
	}
	case MOVE_Swimming:
		//enable swimming up
		fSwimHeightVector = -1.0f;
		break;
	case MOVE_Flying:


		break;
	}
}

void ARemembranceCharacter::CustomStopCrouch()
{
	fSwimHeightVector = 0.0f;
	switch (GetCharacterMovement()->MovementMode)
	{
	case MOVE_None:
		break;
	case MOVE_Walking:
		
		break;
	case MOVE_Falling:


		break;
	case MOVE_Swimming:
		
		break;
	case MOVE_Flying:


		break;
	}
}

void ARemembranceCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		switch (GetCharacterMovement()->MovementMode)
		{
		case MOVE_None:
			break;
		case MOVE_Walking:
			
			GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation;
			AddMovementInput(Direction, Value);
			
			break;
		case MOVE_Falling:
			
			GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation * FMath::Clamp(fJumpingTurnRate, 0.0f, 1.0f);
			AddMovementInput(Direction, Value);
			break;
		case MOVE_Swimming:
		
			GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation;
			AddMovementInput(Direction, Value);
			break;
		case MOVE_Flying:
			
			GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation;
			AddMovementInput(Direction, Value);
			break;
		}
		
	}
}

void ARemembranceCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		switch (GetCharacterMovement()->MovementMode)
		{
		case MOVE_None:
			break;
		case MOVE_Walking:
			GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation;
			AddMovementInput(Direction, Value);
			break;
		case MOVE_Falling:
			GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation * FMath::Clamp(fJumpingTurnRate, 0.0f, 1.0f);
			AddMovementInput(Direction, Value);
			break;
		case MOVE_Swimming:
			GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation;
			AddMovementInput(Direction, Value);
			break;
		case MOVE_Flying:
			GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation;
			AddMovementInput(Direction, Value);
			break;
		}
	}
}


void ARemembranceCharacter::CheckIfWeShouldFly(float DeltaSeconds)
{
	fCurrentFallTime += 1.0f * DeltaSeconds;

	if (fCurrentFallTime >= fFallTimeBeforeFlying)
	{
		OnTimeTriggered();
		if (!bCanFly)
		{
			SwitchMovementType(MOVE_Flying);
			UE_LOG(LogTemp, Warning, TEXT("You are now flying as a bird!"));
		}
	}
}

void ARemembranceCharacter::CheckIfWeShouldStopFlying(float DeltaSeconds)
{
	OnFlyingToWalkingTrigger();

	if (bCanSwitchToWalking)
	{
		fCurrentGroundTime += 1.0f * DeltaSeconds;

		if (fCurrentGroundTime >= GroundTimeBeforeShapeshift)
			SwitchMovementType(MOVE_Walking);
	}
}
