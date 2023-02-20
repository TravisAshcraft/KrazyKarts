// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/VehicleMovementComponent.h"
#include "GameFramework/GameStateBase.h"

// Sets default values for this component's properties
UVehicleMovementComponent::UVehicleMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVehicleMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UVehicleMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


FVector UVehicleMovementComponent::GetDragResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector UVehicleMovementComponent::GetRollingResistance()
{
	
	//Noraml Force is the gravity being applied in the zed axis downward
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	float NormalForce = Mass * AccelerationDueToGravity;
	return -Velocity.GetSafeNormal() * RollingResistance * NormalForce;
}


void UVehicleMovementComponent::SimulateMove(const FVehicleMove Move)
{
	//Add air resistance to force it will eventually zero out
	FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle;
	Force += GetDragResistance();
	Force += GetRollingResistance();
	
	FVector Acceleration = Force / Mass;
	
	// Rolling Resistance = RRCoefficient X NormalForce
	Velocity = Velocity + Acceleration * Move.DeltaTime;

	UpdateLocationFromVelocity(Move.DeltaTime);

	ApplyRotation(Move.DeltaTime, Move.SteeringThrow);
}

FVehicleMove UVehicleMovementComponent::Createmove(float DeltaTime)
{
	FVehicleMove Move;
	Move.DeltaTime = DeltaTime;
	Move.SteeringThrow = SteeringThrow;
	Move.Throttle = Throttle;
	Move.TimeStamp = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	return Move;
}


void UVehicleMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Delta = Velocity * 100 * DeltaTime;
	FHitResult HitResult;
	
	GetOwner()->AddActorWorldOffset(Delta, true, &HitResult);

	if(HitResult.bBlockingHit)
	{
		Velocity = FVector::ZeroVector;
	}
}

void UVehicleMovementComponent::ApplyRotation(float DeltaTime, float Steering)
{
	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinTurningRadius * Steering;
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);

	Velocity = RotationDelta.RotateVector(Velocity);

	GetOwner()->AddActorWorldRotation(RotationDelta);

	UpdateLocationFromVelocity(DeltaTime);
}