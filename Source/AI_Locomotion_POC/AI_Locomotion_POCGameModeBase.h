// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ControlledPawn.h"
#include "Environment.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AI_Locomotion_POCGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class AI_LOCOMOTION_POC_API AAI_Locomotion_POCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<AControlledPawn> PawnToSpawn;

private:
	Environment Env;
	
public:
	// Called at the begin
	virtual void BeginPlay() override;

	// Called to get Env
	Environment& GetEnv();
};
