// Fill out your copyright notice in the Description page of Project Settings.

#include "Remembrance.h"
#include "BaseGeometry.h"


// Sets default values
ABaseGeometry::ABaseGeometry()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = mBaseMesh;

	UserParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleEffect"));
	FootstepSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Footstep sound"));
	UserCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));

	UserParticle->AttachTo(RootComponent);
	UserParticle->bAutoActivate = false;

	FootstepSound->AttachTo(RootComponent);
	FootstepSound->bAutoActivate = false;

	UserCollisionBox->AttachTo(RootComponent);

}

// Called when the game starts or when spawned
void ABaseGeometry::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseGeometry::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

