// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

USTRUCT(BlueprintType)
struct FSpawnPosition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Rotation;

	UPROPERTY()
	float Scale;

	UPROPERTY()
	FVector Location;

};

USTRUCT(BlueprintType)
struct FSpawnCustomizations
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 MinSpawn = 1;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 MaxSpawn = 1;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float MinScale = 1.f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float MaxScale = 1.f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float Radius = 500.f;

};

class UActorPool;

UCLASS()
class TESTFPS_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void PlaceActors(TSubclassOf<AActor> ToSpawn, FSpawnCustomizations Details);

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void PlaceAIPawns(TSubclassOf<APawn> ToSpawn, FSpawnCustomizations Details);

	UFUNCTION(BlueprintCallable, Category = "Pool")
	void SetPool(UActorPool* Pool);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "Navigation")
	FVector NavigationBoundsOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	FVector MinSpawningExtent;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	FVector MaxSpawningExtent;
	
private:
	void PositionNavMeshBoundsVolume();

	// TArray<FSpawnPosition> RandomSpawnPositions(int32 MinSpawn, int32 MaxSpawn, float Radius, float MinScale, float MaxScale);

	bool FindEmptyLocation(FVector& OutLocation, float Radius);

	template<class T>
	void RandomlyPlaceActors(TSubclassOf<T> ToSpawn, FSpawnCustomizations Details);

	void PlaceActor(TSubclassOf<AActor> ToSpawn, FSpawnPosition SpawnPosition);

	void PlaceActor(TSubclassOf<APawn> ToSpawn, FSpawnPosition SpawnPosition);

	bool CanSpawnAtLocation(FVector Location, float Radius);
	
	UActorPool* Pool;

	AActor* NavMeshBoundsVolume;

	//Holds the actors created in the tile and destroys them when the tile is destroyed
	TArray<AActor *> ActorsInTile;

};
