// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseGeometry.generated.h"

UCLASS()
class REMEMBRANCE_API ABaseGeometry : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseGeometry();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	UStaticMeshComponent* mBaseMesh;


	UPROPERTY(EditDefaultsOnly, Category = UserValues)
	UParticleSystemComponent* UserParticle;

	UPROPERTY(EditDefaultsOnly, Category = UserValues)
	UAudioComponent* FootstepSound;

	UPROPERTY(EditDefaultsOnly, Category = StepOnBox)
		UBoxComponent* UserCollisionBox;

private:

	
	
};
