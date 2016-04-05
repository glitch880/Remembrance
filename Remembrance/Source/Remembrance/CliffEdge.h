// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CliffEdge.generated.h"

UCLASS()
class REMEMBRANCE_API ACliffEdge : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACliffEdge();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

private:

	UFUNCTION()
		void BeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void EndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	///** Mesh that will represent the cliff edge. */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "mesh", meta = (AllowPrivateAccess = "true"))
	//	UStaticMeshComponent* CliffMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CollisionBox", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* CollisionBox;

};