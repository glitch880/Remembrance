// Fill out your copyright notice in the Description page of Project Settings.

#include "Remembrance.h"
#include "CliffEdge.h"


// Sets default values
ACliffEdge::ACliffEdge()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACliffEdge::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACliffEdge::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

