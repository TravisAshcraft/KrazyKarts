// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleBase.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AVehicleBase::AVehicleBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

}

// Called when the game starts or when spawned
void AVehicleBase::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		NetUpdateFrequency = 1;
	}
	
}

void AVehicleBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AVehicleBase, ReplicatedLocation);
	DOREPLIFETIME(AVehicleBase, ReplcatedRotation);
	DOREPLIFETIME(AVehicleBase, Velocity);
	DOREPLIFETIME(AVehicleBase, Throttle);
	DOREPLIFETIME(AVehicleBase, SteeringThrow);
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_Authority:
		return "Authority";
	case ROLE_AutonomousProxy:
		return "Autonomous Proxy";
	case ROLE_SimulatedProxy:
		return "Simulated Proxy";
	default:
		return "ERROR";
	}
}


void AVehicleBase::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Delta = Velocity * 100 * DeltaTime;
	FHitResult HitResult;
	
	AddActorWorldOffset(Delta, true, &HitResult);

	if(HitResult.bBlockingHit)
	{
		Velocity = FVector::ZeroVector;
	}
}

void AVehicleBase::ApplyRotation(float DeltaTime)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinTurningRadius * SteeringThrow;
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);

	Velocity = RotationDelta.RotateVector(Velocity);

	AddActorWorldRotation(RotationDelta);

	UpdateLocationFromVelocity(DeltaTime);
}

FVector AVehicleBase::GetDragResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector AVehicleBase::GetRollingResistance()
{
	
	//Noraml Force is the gravity being applied in the zed axis downward
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	float NormalForce = Mass * AccelerationDueToGravity;
	return -Velocity.GetSafeNormal() * RollingResistance * NormalForce;
}




// Called every frame
void AVehicleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//Add air resistance to force it will eventually zero out
	
	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;
	Force += GetDragResistance();
	Force += GetRollingResistance();
	
	FVector Acceleration = Force / Mass;
	
	// Rolling Resistance = RRCoefficient X NormalForce
	Velocity = Velocity + Acceleration * DeltaTime;

	UpdateLocationFromVelocity(DeltaTime);

	ApplyRotation(DeltaTime);

	if(HasAuthority())
	{
		ReplicatedLocation = GetActorLocation();
		ReplcatedRotation = GetActorRotation();
	}

	DrawDebugString(GetWorld(), FVector(0,0, 100), GetEnumText(GetLocalRole()), this, FColor::White, DeltaTime)

	// 1. Create a new Move
	// 2. Save a list of unacknowledged moves
	// 3. Send the move to the server
	// 4. Simulate the move locally
}

// void AVehicleBase::OnRecieveMove()
// 1. Check that the move is valid (No Cheating)
// 2. Simulate the move
// 3. Send the cannonical State to the clients

// void OnRecieveServerState
// 1. Remove all moves incleded in state
// 2. Reset to server state
// 3. Replay/Simulate unacknowledged moves

// Called to bind functionality to input
void AVehicleBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AVehicleBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVehicleBase::MoveRight);
}

void AVehicleBase::OnRep_ReplicatedLocation()
{
	UE_LOG(LogTemp, Warning, TEXT("Replicated Location"));

	SetActorLocation(ReplicatedLocation);
	SetActorRotation(ReplcatedRotation);

}

void AVehicleBase::MoveForward(float Value)
{
	Throttle = Value;
	Server_MoveForward(Value);
}

void AVehicleBase::MoveRight(float Value)
{
	SteeringThrow = Value;
	Server_MoveRight(Value);
}

void AVehicleBase::Server_MoveForward_Implementation(float Value)
{
	Throttle = Value;
}

bool AVehicleBase::Server_MoveForward_Validate(float Value)
{
	return FMath::Abs(Value) <= 1;
}


void AVehicleBase::Server_MoveRight_Implementation(float Value)
{
	SteeringThrow = Value;
}

bool AVehicleBase::Server_MoveRight_Validate(float Value)
{
	return FMath::Abs(Value) <= 1;
}


