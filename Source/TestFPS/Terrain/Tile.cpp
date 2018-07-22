// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "ActorPool.h"
#include "AI/Navigation/NavigationSystem.h"


ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = false;

	// FVector to offset by when checking out a NavMesh from the pool.
	NavigationBoundsOffset = FVector(2000, 0, 0);

	// Spawning box dimensions.
	MinSpawningExtent = FVector(0, -2000, 0);
	MaxSpawningExtent = FVector(4000, 2000, 0);

}

void ATile::SetPool(UActorPool* InPool)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Setting Pool %s"), *(this->GetName()), *(InPool->GetName()));
	Pool = InPool;

	PositionNavMeshBoundsVolume();

}

void ATile::PositionNavMeshBoundsVolume()
{
	NavMeshBoundsVolume = Pool->Checkout();
	if (NavMeshBoundsVolume == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Not enough actors in pool."), *GetName());
		return;

	}
	// Checkout NavMeshBoundsVolume and offset its location, then build navigation.
	UE_LOG(LogTemp, Error, TEXT("[%s] Checked out: {%s}"), *GetName(), *NavMeshBoundsVolume->GetName());
	NavMeshBoundsVolume->SetActorLocation(GetActorLocation() + NavigationBoundsOffset);
	GetWorld()->GetNavigationSystem()->Build();

}

void ATile::BeginPlay()
{
	Super::BeginPlay();

}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Pool != nullptr && NavMeshBoundsVolume != nullptr)
	{
		Pool->Retrieve(NavMeshBoundsVolume);

	}

	// Delete the ActorsInTile from memory when game ends.
	if (ActorsInTile.Num() != 0)
	{
		AActor* Prop;
		while (ActorsInTile.Num() != 0)
		{
			Prop = ActorsInTile.Pop();
			Prop->Destroy();

		}

	}

}

void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Template for PlaceActors, allowing you to place Pawns or Actors.
template<class T>
void ATile::RandomlyPlaceActors(TSubclassOf<T> ToSpawn, FSpawnCustomizations Details)
{
	// Set random points in the box as our SpawnPoint
	int32 NumberToSpawn = FMath::RandRange(Details.MinSpawn, Details.MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++)
	{
		// Randomize scale of actor to spawn.
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(Details.MinScale, Details.MaxScale);
		bool found = FindEmptyLocation(SpawnPosition.Location, Details.Radius * SpawnPosition.Scale);
		if (found)
		{
			// Randomize rotation of actor to spawn.
			SpawnPosition.Rotation = FMath::RandRange(-180.f, 180.f);
			PlaceActor(ToSpawn, SpawnPosition);

		}

	}

}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, FSpawnCustomizations Details)
{
	RandomlyPlaceActors(ToSpawn, Details);

}

void ATile::PlaceAIPawns(TSubclassOf<APawn> ToSpawn, FSpawnCustomizations Details)
{
	RandomlyPlaceActors(ToSpawn, Details);

}

bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius)
{
	// Create the box
	FBox SpawnVolumeBox(MinSpawningExtent, MaxSpawningExtent);

	const int32 MAX_ATTEMPTS = 100;
	for (size_t i = 0; i < MAX_ATTEMPTS; i++)
	{
		/**
		 * CanSpawnAtLocation returns true if its sweep doesn't detect other actors.
		 * CandidatePoint is a position in local space, relative to the SpawnVolumeBox.
		 * CandidatePoint is converted from local space to global space in CanSpawnAtLocation.
		 *
		 */
		FVector CandidatePoint = FMath::RandPointInBox(SpawnVolumeBox);
		if (CanSpawnAtLocation(CandidatePoint, Radius))
		{
			OutLocation = CandidatePoint;
			return true;

		}

	}
	return false;

}

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FSpawnPosition SpawnPosition)
{
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ToSpawn);
	if (SpawnedActor)
	{
		// Set actor's rotation and scale when placed.
		ActorsInTile.Add(SpawnedActor);
		SpawnedActor->SetActorRelativeLocation(SpawnPosition.Location);
		SpawnedActor->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		SpawnedActor->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));
		SpawnedActor->SetActorScale3D(FVector(SpawnPosition.Scale));

	}

}

void ATile::PlaceActor(TSubclassOf<APawn> ToSpawn, FSpawnPosition SpawnPosition)
{
	// Set pawn's rotation when placed. Tag pawn with "Enemy" (intended for enemy AI pawns).
	FRotator Rotation = FRotator(0, SpawnPosition.Rotation, 0);
	APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(ToSpawn, SpawnPosition.Location, Rotation);
	if (SpawnedPawn)
	{
		ActorsInTile.Add(SpawnedPawn);
		SpawnedPawn->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		SpawnedPawn->SpawnDefaultController();
		SpawnedPawn->Tags.Add(FName("Enemy"));

	}

}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius)
{
	// Convert location from local space to global space.
	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
	// Sweep for actors in the radius on collision channel: ECC_GameTraceChannel2.
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius)
	);
	return !HasHit;

}
