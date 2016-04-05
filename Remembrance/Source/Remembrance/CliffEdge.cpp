// Fill out your copyright notice in the Description page of Project Settings.

#include "Remembrance.h"
#include "RemembranceCharacter.h"
#include "Engine.h"
#include "CliffEdge.h"


// Sets default values
ACliffEdge::ACliffEdge()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	////Create the static mesh for the cliff
	//CliffMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//CliffMesh->AttachTo(RootComponent);
	//RootComponent = CliffMesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACliffEdge::BeginOverlap);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ACliffEdge::EndOverlap);
	RootComponent = CollisionBox;

	

	

}

// Called when the game starts or when spawned
void ACliffEdge::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACliffEdge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACliffEdge::BeginOverlap(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(ARemembranceCharacter::StaticClass()))
	{
		ARemembranceCharacter* PlayerChar = Cast<ARemembranceCharacter>(OtherActor);
		PlayerChar->SetIsOnEdge(true);
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Player entered cliff"));
	}

}

void ACliffEdge::EndOverlap(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ARemembranceCharacter::StaticClass()))
	{
		ARemembranceCharacter* PlayerChar = Cast<ARemembranceCharacter>(OtherActor);
		PlayerChar->SetIsOnEdge(false);
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Player exited cliff"));
	}
}