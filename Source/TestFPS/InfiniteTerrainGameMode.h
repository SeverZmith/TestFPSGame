// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestFPSGameMode.h"
#include "InfiniteTerrainGameMode.generated.h"


UCLASS()
class TESTFPS_API AInfiniteTerrainGameMode : public ATestFPSGameMode
{
	GENERATED_BODY()
	
public:
	AInfiniteTerrainGameMode();

	UFUNCTION(BlueprintCallable, Category = "Bounds Pool")
	void PopulateBoundsVolumePool();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
	class UActorPool* NavMeshBoundsVolumePool;
	
private:
	void AddToPool(class ANavMeshBoundsVolume *VolumeToAdd);

};
