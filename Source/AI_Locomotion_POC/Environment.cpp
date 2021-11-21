// Fill out your copyright notice in the Description page of Project Settings.

#include "Environment.h"

Environment::Environment(UWorld* _World, TSubclassOf<AControlledPawn> _PawnToSpawn)
{
	// Set World
	World = _World;

	// Set PawntoSpawn
	PawnToSpawn = _PawnToSpawn;

	// Initialize float
	muscleStrength = 1e5;

	// Spawn new actor at location
	FVector Loc = FVector(390.0f, 0.0f, 200.0f);
	FRotator Rot = FRotator(0.0f, 180.0f, 0.0f);

	SpawnPawn(Loc, Rot);
}

Environment::Environment()
{
}

Environment::~Environment()
{
}

void Environment::step(const std::vector<float>& action, std::vector<float>& state, float& reward)
{
	// Perform Action
	TArray<float> arr = VectorToArray<float>(action);
	Pawn->MoveSkeleton(arr, muscleStrength);

	// Calculate reward
	reward = Pawn->GetMesh()->GetSkeletalCenterOfMass().Z / 100;

	// Get new State
	TArray<float> JointLocations;
	Pawn->GetJointLocations(JointLocations, Pawn->BoneNames);
	state = ArrayToVector(JointLocations);
}

void Environment::reset()
{
	
	// Kill old Pawn
	if (Pawn)
	{
		World->DestroyActor(Pawn);
	}

	// Spawn new actor at location
	FVector Loc = FVector(390.0f, 0.0f, 200.0f);
	FRotator Rot = FRotator(0.0f, 180.0f, 0.0f);

	SpawnPawn(Loc, Rot);
}

void Environment::SpawnPawn(FVector Loc, FRotator Rot)
{
	if (PawnToSpawn) {
		FActorSpawnParameters SpawnParamters;

		Pawn = World->SpawnActor<AControlledPawn>(PawnToSpawn, Loc, Rot, SpawnParamters);
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("PawnToSpawn is not set"));
	}
}

