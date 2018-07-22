// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorPool.h"


UActorPool::UActorPool()
{
	PrimaryComponentTick.bCanEverTick = false;

}

AActor* UActorPool::Checkout()
{
	if (Pool.Num() == 0)
	{
		return nullptr;

	}
	return Pool.Pop();

}

void UActorPool::Retrieve(AActor* ActorToRetrieve)
{
	Add(ActorToRetrieve);

}

void UActorPool::Add(AActor* ActorToAdd)
{
	Pool.Push(ActorToAdd);

}
