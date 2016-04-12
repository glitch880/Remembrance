// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Remembrance.h"
#include "FlyingComp.h"
#include "RemembranceCharacter.h"

#include "Engine.h" //TODO remove this

//////////////////////////////////////////////////////////////////////////
// ARemembranceCharacter

ARemembranceCharacter::ARemembranceCharacter()

{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	RootComponent = GetCapsuleComponent();

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
	CameraBoom->TargetArmLength = 1200.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	FlyingComponent = CreateDefaultSubobject<UFlyingComp>(TEXT("FlyingComponent"));
	AddOwnedComponent(FlyingComponent);
	//FlyingComponent->RegisterComponent();
	

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


	//Swimming setup
	fSwimHeightVector = 0.0f;
	fTransformedSwimSpeed = 600.f;
	bSubmerged = false;
	bIsTransformedWater = false;
	fBreathTime = 10;
	bHasJumpedInWater = false;
	fJumpOutOfWaterMaxVelocity = 600;
	fMinWaterJumpVelocity = 1600;
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

void ARemembranceCharacter::SwitchMovementType(EMovementMode Mode)
{
	GetCharacterMovement()->SetMovementMode(Mode);

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
		case MOVE_Custom:
		{

			break;
		}
	}

}


void ARemembranceCharacter::Tick(float DeltaSeconds)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Current Z velocity %f"), GetCharacterMovement()->Velocity.Z));
	switch (GetCharacterMovement()->MovementMode)
	{
	case MOVE_None:
		break;
	case MOVE_Walking:
		fCurrentTimeSubmerged = 0;
		break;
	case MOVE_Falling:
	{
		//If we are falling then we should check if we can start flying instead.
		CheckIfWeShouldFly(DeltaSeconds);
		//bHasJumpedInWater = false;
		break;
	}
	case MOVE_Swimming:
	{
		//TODO increase time below surface if character  almost fully submerged. Potential solution : use line tract straight up and compare distance with a threshold.
		CheckSubmerged();
		if (bSubmerged) //change to condition
			fCurrentTimeSubmerged += 1.0f * DeltaSeconds;
		else
			fCurrentTimeSubmerged = 0;

		if (fCurrentTimeSubmerged > fBreathTime && !bIsTransformedWater) //transform
			TransformWater();
		

		//enable swimming up
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation(0, Rotation.Yaw, 0);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Z);
		AddMovementInput(Direction, fSwimHeightVector*fTransformedSwimSpeed*DeltaSeconds);  //TODO should not use transformed speed here

		//if not submerged
		if (!bSubmerged && fSwimHeightVector > 0 && GetCharacterMovement()->Velocity.Z >= 0  && !bHasJumpedInWater)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Activating jump out of water  %f %f"), GetCharacterMovement()->Velocity.Z, GetVelocity().Z));
			//float vel = GetVelocity().Z * 2;
			//if (vel < fMinWaterJumpVelocity)
				//vel = fMinWaterJumpVelocity;

			//GetCharacterMovement()->AddImpulse(FVector(0, 0, GetCharacterMovement()->Velocity.Z*10)*fSwimHeightVector, true);
			//GetCharacterMovement()->Velocity.Z += vel;
			//GetCharacterMovement()->AddImpulse(FVector(0, 0, 5000.f), true);

			GetCharacterMovement()->Velocity.Z += 2000;

			GetCharacterMovement()->Velocity.Z =  FMath::Clamp(GetCharacterMovement()->Velocity.Z, 2000.f, 6000.f);

		//	AddMovementInput(Direction, 4000.f);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("after activation  %f %f"), GetCharacterMovement()->Velocity.Z, GetVelocity().Z));
			bHasJumpedInWater = true;
			//LaunchCharacter((FVector(0, 0, GetCharacterMovement()->Velocity.Z )*fSwimHeightVector), true, true);
			//GetCharacterMovement()->Launch(FVector(0, 0, GetCharacterMovement()->Velocity.Z)*fSwimHeightVector);
		}
		else if (bSubmerged)
			bHasJumpedInWater = false;
		break;
	}
	case MOVE_Flying:
		
		UE_LOG(LogTemp, Warning, TEXT("Normalize Camera Pitch: %f Roll: %f"), CameraBoom->RelativeRotation.Pitch, CameraBoom->RelativeRotation.Roll); //TODO smooth camera movement 
		CameraBoom->SetRelativeRotation(FMath::RInterpTo(CameraBoom->RelativeRotation, FRotator(0, 0, 0), GetWorld()->GetDeltaSeconds(), 2.f));
		UE_LOG(LogTemp, Warning, TEXT("Normalize camera Pitch: %f Roll: %f"), CameraBoom->RelativeRotation.Pitch, CameraBoom->RelativeRotation.Roll);
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, 1200.f, GetWorld()->GetDeltaSeconds(), 2.f); //TODO this fixes the "popping" of the camera boom but if you pitch out of the ground you dont get the boom back

		GetController()->SetControlRotation(FRotator(CameraBoom->GetComponentRotation().Pitch, GetActorRotation().Yaw, CameraBoom->GetComponentRotation().Roll));

		//If we are flying, we should constantly check if we should continue or stop.
		CheckIfWeShouldStopFlying(DeltaSeconds);
		FlyingComponent->MyOwnTick(DeltaSeconds);

		//FlyingCollisionTrigger();

		break;
	case MOVE_Custom:
		//tick for custom
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

void ARemembranceCharacter::FlyingCollisionTrigger_Implementation()
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
	//	UE_LOG(LogTemp, Warning, TEXT("Falling, can't jump"));
		break;
	case MOVE_Swimming:
		//enable swimming up
		//UE_LOG(LogTemp, Warning, TEXT("Swimming up"));
		fSwimHeightVector = 1.0f;
		
		break;
	case MOVE_Flying:

		
		break;
	case MOVE_Custom:
		//tick for custom
		
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
	case MOVE_Custom:
		//tick for custom
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

		UE_LOG(LogTemp, Warning, TEXT("You are now custom!"));
		//GetCharacterMovement()->SetMovementMode(MOVE_Custom);

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
	case MOVE_Custom:
		//tick for custom
		
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
	case MOVE_Custom:
		//tick for custom
		//GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		break;
	}
}

void ARemembranceCharacter::MoveForward(float Value)
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
			if ((Controller != NULL) && (Value != 0.0f))
			{
				GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation;
				AddMovementInput(Direction, Value);
			}
			
			break;
		case MOVE_Falling:
			if ((Controller != NULL) && (Value != 0.0f))
			{
				GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation * FMath::Clamp(fJumpingTurnRate, 0.0f, 1.0f);
				AddMovementInput(Direction, Value);
			}
			break;
		case MOVE_Swimming:
			if ((Controller != NULL) && (Value != 0.0f))
			{
				GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation;
				AddMovementInput(Direction, Value);
			}
			break;
		case MOVE_Flying:
			
			FlyingComponent->MoveUpInput(Value);
			break;
		case MOVE_Custom:
		{
			GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation;
			const FRotator RotationC = this->GetActorRotation();// Controller->GetControlRotation();
			//this->GetMesh()->contrains
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Aliens, %f, %f"), RotationC.Yaw));
			
			const FRotator YawRotationC(0, RotationC.Yaw, 0);
			// get forward vector
			const FVector DirectionC = FRotationMatrix(YawRotationC).GetUnitAxis(EAxis::X);
			AddMovementInput(DirectionC, Value);

			break;
		}
		}
		
}

void ARemembranceCharacter::MoveRight(float Value)
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
			if ((Controller != NULL) && (Value != 0.0f))
			{
				GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation;
				AddMovementInput(Direction, Value);
			}
			break;
		case MOVE_Falling:
			if ((Controller != NULL) && (Value != 0.0f))
			{
				GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation * FMath::Clamp(fJumpingTurnRate, 0.0f, 1.0f);
				AddMovementInput(Direction, Value);
			}
			break;
		case MOVE_Swimming:
			if ((Controller != NULL) && (Value != 0.0f))
			{
				GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation;
				AddMovementInput(Direction, Value);
			}
			break;
		case MOVE_Flying:

			FlyingComponent->MoveRightInput(Value);
			/*GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation;
			AddMovementInput(Direction, Value);*/
			break;
		case MOVE_Custom:
		//	GetCharacterMovement()->RotationRate.Yaw = StandardYawRotation;
		//	AddMovementInput(Direction, Value);
			break;
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
			GetCapsuleComponent()->SetSimulatePhysics(true);
			CameraBoom->bUsePawnControlRotation = false;
		//	CameraBoom->TargetArmLength = 1200.f;
			UE_LOG(LogTemp, Warning, TEXT("You are now flying as a bird!"));

			//CameraBoom->SetRelativeRotation(FRotator( 0, 0, 0));
		}
	}
}

void ARemembranceCharacter::CheckIfWeShouldStopFlying(float DeltaSeconds)
{
	OnFlyingToWalkingTrigger();

	if (bCanSwitchToWalking)
	{
		fCurrentGroundTime += 1.0f * DeltaSeconds;
		if(GetActorRotation().Pitch < 0 || GetActorRotation().Pitch > 230.f)
			SetActorRotation(FMath::RInterpTo(GetActorRotation(), FRotator(0, GetActorRotation().Yaw, GetActorRotation().Roll), GetWorld()->GetDeltaSeconds(), 10.0f));
		if(GetActorRotation().Roll  > 10 || GetActorRotation().Roll < -10 )
			SetActorRotation(FMath::RInterpTo(GetActorRotation(), FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw, 0), GetWorld()->GetDeltaSeconds(), 10.0f));


	//	SetActorRotation(FMath::RInterpTo(GetActorRotation(), FRotator(0, GetActorRotation().Yaw, 0), GetWorld()->GetDeltaSeconds(), 20.0f));
		UE_LOG(LogTemp, Warning, TEXT("Normalize Pitch: %f Roll: %f"), GetActorRotation().Pitch, GetActorRotation().Roll);
		
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, 600.f, GetWorld()->GetDeltaSeconds(), 10.f); //TODO this fixes the "popping" of the camera boom but if you pitch out of the ground you dont get the boom back
	
		if (fCurrentGroundTime >= GroundTimeBeforeShapeshift)
		{
			SwitchMovementType(MOVE_Walking);
			GetCapsuleComponent()->SetSimulatePhysics(false);
			CameraBoom->bUsePawnControlRotation = true;
			
			CameraBoom->TargetArmLength = 600.f;
			CameraBoom->bInheritRoll = true;  //make the camera straight
			//CameraBoom->SetWorldRotation(FRotator(0, GetActorRotation().Yaw, 0));
		//	CameraBoom->RelativeRotation.Yaw = GetActorRotation().Yaw;
			GetController()->SetControlRotation(FRotator(0, GetActorRotation().Yaw,0));  //enough to set the camera behind the character as it lands

			//FRotator temp = FMath::RInterpTo(GetActorRotation(), FRotator(0, 0, 0), DeltaSeconds, 1.0f);
			//SetActorRotation(FMath::RInterpTo(GetActorRotation(), FRotator(0, 0, 0), GetWorld()->GetDeltaSeconds(), 10.0f));
			SetActorRotation(FRotator(0, GetActorRotation().Yaw, 0));
		}
	}
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, 1200.f, GetWorld()->GetDeltaSeconds(), 10.f); //TODO test
}
//Transform into water creature. Change swim speed, model, anim, effects mm.
void ARemembranceCharacter::TransformWater()
{
	//TODO placeholder
	UE_LOG(LogTemp, Warning, TEXT("You are  water creature"));
	GetCharacterMovement()->MaxSwimSpeed = fTransformedSwimSpeed;
}


void ARemembranceCharacter::SetLengthToObject(float Lenght)
{
	FlyingComponent->LenghtToObject = Lenght;
}

void ARemembranceCharacter::SetCollisionDetected(bool Collision)
{
	FlyingComponent->bCollisionDetected = Collision;
}

void ARemembranceCharacter::SetPushValue(float value)
{
	FlyingComponent->PushValue = value;
}
