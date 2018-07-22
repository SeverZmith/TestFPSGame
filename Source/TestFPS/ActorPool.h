// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorPool.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTFPS_API UActorPool : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActorPool();
	
	// Pop actor from pool
	AActor* Checkout();

	// Add actor to push
	void Retrieve(AActor* ActorToRetrieve);

	// Push actor to pool
	void Add(AActor* ActorToAdd);

private:
	TArray<AActor*> Pool;

};
