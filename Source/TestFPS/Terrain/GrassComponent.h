// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GrassComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TESTFPS_API UGrassComponent : public UHierarchicalInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGrassComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, Category = Spawning)
	FBox SpawningExtents;

	UPROPERTY(EditDefaultsOnly, Category = Spawning)
	int SpawnCount;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	void SpawnGrass();

};
