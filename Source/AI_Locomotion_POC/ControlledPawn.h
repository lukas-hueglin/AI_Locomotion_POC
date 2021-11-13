// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ControlledPawn.generated.h"

UCLASS()
class AI_LOCOMOTION_POC_API AControlledPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AControlledPawn();

public:
	// Capsule component
	UPROPERTY(VisibleDefaultsOnly, Category = "Root")
	UCapsuleComponent* Capsule;

	// Mesh component
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* Mesh;

	// Arrow component
	UPROPERTY(VisibleDefaultsOnly, Category = "Root")
	UArrowComponent* Arrow;

	// Spring arm component
	UPROPERTY(VisibleDefaultsOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	// Camera component
	UPROPERTY(VisibleDefaultsOnly, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(BlueprintReadWrite, Category = "MucleContraction")
	float muscleActivation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	/*
	*	Called to move the joint between the socket and it's parent
	*	The torque vector is realative to the socket's local space
	*	Method has to be called every tick if a constant torque is wanted
	*/
	void MoveJoint(FName BoneName, EAxis::Type Axis, float force);

	void ControlMuscleActivation(float Val);

public:
	// Called to get the pawn mesh
	UFUNCTION()
	USkeletalMeshComponent* GetMesh();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
