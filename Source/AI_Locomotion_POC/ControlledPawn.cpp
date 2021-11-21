// Fill out your copyright notice in the Description page of Project Settings.

#include "ControlledPawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "AI_Locomotion_POCGameModeBase.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Math/Vector.h"

#include <vector>
#include <cstdlib>

// Sets default values
AControlledPawn::AControlledPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create capsule component
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComponent);
	Capsule->InitCapsuleSize(11.0f, 96.0f);
	Capsule->SetVisibility(true, true);

	// Create mesh component
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Capsule);
	Mesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -89.0f));
	Mesh->bCastDynamicShadow = true;

	// Create arrow component
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(Capsule);

	// Create apring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(Capsule);

	// Create apring arm component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = true;

	// Initialize float
	muscleActivation = 0.0f;
}

// Called when the game starts or when spawned
void AControlledPawn::BeginPlay()
{
	Super::BeginPlay();

	// Initialize arrays
	GetMesh()->GetBoneNames(BoneNames);

	// Enable Ragdoll
	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), true, false);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	// Enable player input
	EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

// Called to move the joint between the socket and it's parent in a single axis
void AControlledPawn::MoveJoint(FName BoneName, EAxis::Type Axis, float force)
{
	// Create variables for bone names
	FName p_Name = GetMesh()->GetParentBone(BoneName);
	FName c_Name = BoneName;

	// Get Bone Locations
	FVector p_Location = GetMesh()->GetBoneLocation(p_Name);
	FVector c_Location = GetMesh()->GetBoneLocation(c_Name);

	// Get normal and forward vectors
	FVector p_Forward = GetMesh()->GetBoneAxis(p_Name, EAxis::Y);
	FVector c_Normal = GetMesh()->GetBoneAxis(c_Name, Axis);
	FVector p_Normal = - (c_Normal - FVector::DotProduct(c_Normal, p_Forward) * p_Forward);
	p_Normal.Normalize();

	// Get center of mass and vectors from the bone positions to the com
	FVector CenterOfMass = GetMesh()->GetSkeletalCenterOfMass();
	FVector p_ToCenterOfMass = p_Location - CenterOfMass;
	FVector c_ToCenterOfMass = c_Location - CenterOfMass;

	// Get length lever arm of both bones
	float p_angle = FMath::Acos(FVector::DotProduct(p_ToCenterOfMass, p_Normal) / p_ToCenterOfMass.Size());
	float p_la = sin(p_angle) * p_ToCenterOfMass.Size();
	float c_angle = FMath::Acos(FVector::DotProduct(c_ToCenterOfMass, c_Normal) / c_ToCenterOfMass.Size());
	float c_la = sin(c_angle) * c_ToCenterOfMass.Size();

	// Calculate torques
	FVector p_Torque = (force * c_la * p_Normal) / p_la;
	FVector c_Torque = force * c_Normal;

	//UKismetSystemLibrary::DrawDebugArrow(Mesh, Mesh->GetBoneTransform(Mesh->GetBoneIndex(FName(p_Name))).GetLocation(), Mesh->GetBoneTransform(Mesh->GetBoneIndex(FName(p_Name))).GetLocation() + p_Torque / force * 100, 1.0f, FLinearColor::Red, 0.0f, 1.0f);
	//UKismetSystemLibrary::DrawDebugArrow(Mesh, Mesh->GetBoneTransform(Mesh->GetBoneIndex(FName(c_Name))).GetLocation(), Mesh->GetBoneTransform(Mesh->GetBoneIndex(FName(c_Name))).GetLocation() + c_Torque / force * 100, 1.0f, FLinearColor::Blue, 0.0f, 1.0f);

	Mesh->AddTorqueInRadians(p_Torque, p_Name, false);
	Mesh->AddTorqueInRadians(c_Torque, c_Name, false);
}

void AControlledPawn::ControlMuscleActivation(float Val)
{
	muscleActivation = Val;
}

USkeletalMeshComponent* AControlledPawn::GetMesh()
{
	return Mesh;
}

// Called every frame
void AControlledPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AAI_Locomotion_POCGameModeBase* GameMode = (AAI_Locomotion_POCGameModeBase*)GetWorld()->GetAuthGameMode();

	std::vector<float> action(204, 0.0f);
	std::vector<float> state(204, 0.0f);
	float reward;

	for (int i = 0; i < action.size(); ++i)
	{
		action.at(i) = 2 * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 1;
	}
	
	GameMode->GetEnv().step(action, state, reward);
}

// Called to bind functionality to input
void AControlledPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MuscleControl"), this, &AControlledPawn::ControlMuscleActivation);
}

// Called to move the hole skeleton
void AControlledPawn::MoveSkeleton(const TArray<float>& Activation, float strength)
{
	// Check if valid activation num
	if (Activation.Num() / 3 == BoneNames.Num())
	{
		for (int i = 0; i < Activation.Num(); i += 3)
		{
			MoveJoint(BoneNames[i / 3], EAxis::X, Activation[i] * strength);
			MoveJoint(BoneNames[i / 3], EAxis::Y, Activation[i+1] * strength);
			MoveJoint(BoneNames[i / 3], EAxis::Z, Activation[i+2] * strength);
		}	
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Size of activation array is not right"));
	}
}

void AControlledPawn::GetJointLocations(TArray<float>& JointLocations, TArray<FName> BoneList)
{
	for (FName Bone : BoneList)
	{
		FVector Location = GetMesh()->GetBoneLocation(Bone, EBoneSpaces::WorldSpace);
		
		JointLocations.Add(Location.X);
		JointLocations.Add(Location.Y);
		JointLocations.Add(Location.Z);
	}
}

