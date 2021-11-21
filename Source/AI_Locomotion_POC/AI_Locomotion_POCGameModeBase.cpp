// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI_Locomotion_POCGameModeBase.h"

void AAI_Locomotion_POCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// enable tick
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	
	// Create environment
	Env = Environment(GetWorld(), PawnToSpawn);
}

Environment& AAI_Locomotion_POCGameModeBase::GetEnv()
{
	return Env;
}
