// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "ActorPool.h"
#include "AI/Navigation/NavigationSystem.h"


// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NavigationBoundsOffset = FVector(2000, 0, 0);

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
	UE_LOG(LogTemp, Error, TEXT("[%s] Checked out: {%s}"), *GetName(), *NavMeshBoundsVolume->GetName());
	NavMeshBoundsVolume->SetActorLocation(GetActorLocation() + NavigationBoundsOffset);
	GetWorld()->GetNavigationSystem()->Build();
}

// Called when the game starts or when spawned
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

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

template<class T>
void ATile::RandomlyPlaceActors(TSubclassOf<T> ToSpawn, FSpawnCustomizations Details)
{
	// Set random points in the box as our SpawnPoint
	int32 NumberToSpawn = FMath::RandRange(Details.MinSpawn, Details.MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++)
	{
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(Details.MinScale, Details.MaxScale);
		bool found = FindEmptyLocation(SpawnPosition.Location, Details.Radius * SpawnPosition.Scale);
		if (found)
		{
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
		ActorsInTile.Add(SpawnedActor);
		SpawnedActor->SetActorRelativeLocation(SpawnPosition.Location);
		SpawnedActor->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		SpawnedActor->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));
		SpawnedActor->SetActorScale3D(FVector(SpawnPosition.Scale));
	}
}

void ATile::PlaceActor(TSubclassOf<APawn> ToSpawn, FSpawnPosition SpawnPosition)
{
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
	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
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
