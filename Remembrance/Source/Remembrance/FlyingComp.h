// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "FlyingComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMEMBRANCE_API UFlyingComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFlyingComp();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	/** Bound to the vertical axis */
	void MoveUpInput(float Val);

	/** Bound to the horizontal axis */
	void MoveRightInput(float Val);

	UPROPERTY(BlueprintReadWrite, Category = Flying)
		bool bCollisionDetected;

	UPROPERTY(BlueprintReadWrite, Category = Flying)
		float LenghtToObject;

	UPROPERTY(BlueprintReadWrite, Category = Flying)
		float PushValue;

	void MyOwnTick(float DeltaTime);

private:

	/** Max forward speed */
	UPROPERTY(Category = Speed, EditAnywhere)
		float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(Category = Speed, EditAnywhere)
		float MinSpeed;

	/** How much speed you gain/loose by going up and down */
	UPROPERTY(Category = Speed, EditAnywhere)
		float PitchVelocityGain;

	/** How much speed you gain/loose by going up and down */
	UPROPERTY(Category = Speed, EditAnywhere)
		float PitchVelocityLoss;

	/** The speed which will decrease each second */
	UPROPERTY(Category = Speed, EditAnywhere)
		float SpeedLossEachSecond;

	/** How quickly you can steer this is combined with the hardness */
	UPROPERTY(Category = GeneralTurnControl, EditAnywhere)
		float TurnSpeed;

	/** Determines how hard the plane should start rolling, higher values equals harder roll */
	UPROPERTY(Category = GeneralTurnControl, EditAnywhere)
		float RollHardness;

	/** Determines how hard the plane should start yawing, higher values equals harder yaw */
	UPROPERTY(Category = GeneralTurnControl, EditAnywhere)
		float YawHardness;

	/** Determines how hard the plane should start pitching, higher values equals harder pitch */
	UPROPERTY(Category = GeneralTurnControl, EditAnywhere)
		float PitchHardness;

	/** How much to scale the roll when rolling up and down  */
	UPROPERTY(Category = Roll, EditAnywhere)
		float RollScale;

	/** How much to scale the yaw turning the plane */
	UPROPERTY(Category = Yaw, EditAnywhere)
		float YawScale;

	/** How much to scale the pitch when pitching the plane */
	UPROPERTY(Category = Pitch, EditAnywhere)
		float PitchScale;

	/** A Scale that determines how fast the plane should roll back to no angle. */
	UPROPERTY(Category = Roll, EditAnywhere)
		float NormalizeRollScale;

	/** the maxiumum angle that you can pitch upwards with the plane */
	UPROPERTY(Category = Pitch, EditAnywhere)
		float MaxPitchUpAngle;

	/** The maximum angle that you can roll with the plane */
	UPROPERTY(Category = Roll, EditAnywhere)
		float MaxRollAngle;

	/** How fast the plane will pitch down when having no forward speed. */
	UPROPERTY(Category = Pitch, EditAnywhere)
		float PitchForce;

	/** The Strenght which the wall will push the player each second */
	UPROPERTY(Category = Collision, EditAnywhere)
		float WallCollisionPushStrenght;


	/** The Strenght which the wall will push the player each second */
	UPROPERTY(Category = Collision, EditAnywhere)
		float MaxPitchStrenght;


	/** The Strenght which the wall will push the player each second */
	UPROPERTY(Category = Collision, EditAnywhere)
		float MaxYawStrenght;

	/** The Strenght which the wall will push the player each second */
	UPROPERTY(Category = Collision, EditAnywhere)
		float MaxRollStrenght;

	/** The Strenght which the wall will push the player each second */
	UPROPERTY(Category = Collision, EditAnywhere)
		float TraceLenght;

	/** The minimum force which will push the plane down each second*/
	UPROPERTY(Category = Pitch, EditAnywhere)
		float MinPitchdownForce;

	/** The maximum force which will push the plane down each second*/
	UPROPERTY(Category = Pitch, EditAnywhere)
		float MaxPitchdownForce;


	/** Current forward speed */
	float CurrentForwardSpeed;

	/** Current yaw speed */
	float CurrentYawSpeed;

	/** Current pitch speed */
	float CurrentPitchSpeed;

	/** Current pitch speed */
	float CurrentPitchTurnSpeed;

	/** Bool that determines if we are turning or not */
	bool bIsTurning;

	/** Current roll speed */
	float CurrentRollSpeed;

	/** Function that is being called to check if we are colliding with a wall or not*/
	void WallRayCasting();
	
};
