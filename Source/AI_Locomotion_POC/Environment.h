// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "ControlledPawn.h"
#include "CoreMinimal.h"

/**
 * 
 */

class AI_LOCOMOTION_POC_API Environment
{
public:
	Environment(UWorld* _World, TSubclassOf<AControlledPawn> _PawnToSpawn);
	Environment();
	~Environment();

private:
	AControlledPawn* Pawn;
	TSubclassOf<AControlledPawn> PawnToSpawn;

	UWorld* World;

	float muscleStrength;

protected:
	template<typename T>
	TArray<T> VectorToArray(std::vector<T> vec);

	template<typename T>
	std::vector<T> ArrayToVector(TArray<T> arr);

public:
	// Called from the agent to make a step
	void step(const std::vector<float>& action, std::vector<float>& state, float& reward);

	// Called to reset the scene
	void reset();

	// Called to spawn the pawn
	void SpawnPawn(FVector Loc, FRotator Rot);
};

template<typename T>
inline TArray<T> Environment::VectorToArray(std::vector<T> vec)
{
	TArray<T> arr;

	for (T element : vec)
	{
		arr.Add(element);
	}

	return arr;
}

template<typename T>
inline std::vector<T> Environment::ArrayToVector(TArray<T> arr)
{
	std::vector<T> vec;

	for (T element : arr)
	{
		vec.push_back(element);
	}

	return vec;
}
