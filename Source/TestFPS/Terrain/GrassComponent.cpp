// Fill out your copyright notice in the Description page of Project Settings.

#include "GrassComponent.h"


UGrassComponent::UGrassComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UGrassComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnGrass();

}

void UGrassComponent::SpawnGrass()
{
	// Create instances of grass equal to the SpawnCount at random location in spawning box.
	for (size_t i = 0; i < SpawnCount; i++)
	{
		FVector Location = FMath::RandPointInBox(SpawningExtents);
		AddInstance(FTransform(Location));

	}

}

void UGrassComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
