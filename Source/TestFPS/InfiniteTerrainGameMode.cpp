// Fill out your copyright notice in the Description page of Project Settings.

#include "InfiniteTerrainGameMode.h"
#include "AI/Navigation/NavMeshBoundsVolume.h"
#include "EngineUtils.h"
#include "ActorPool.h"

AInfiniteTerrainGameMode::AInfiniteTerrainGameMode()
{
	// Create actor pool
	NavMeshBoundsVolumePool = CreateDefaultSubobject<UActorPool>(FName("Nav Mesh Bounds Volume Pool"));

}

void AInfiniteTerrainGameMode::PopulateBoundsVolumePool()
{
	// Create iterator for actor pool
	auto NavMeshVolumeIterator = TActorIterator<ANavMeshBoundsVolume>(GetWorld());
	while (NavMeshVolumeIterator)
	{
		AddToPool(*NavMeshVolumeIterator);
		++NavMeshVolumeIterator;

	}

}

void AInfiniteTerrainGameMode::AddToPool(ANavMeshBoundsVolume *VolumeToAdd)
{
	// Add volume to pool
	NavMeshBoundsVolumePool->Add(VolumeToAdd);

}
