// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleBase.h"

// Sets default values
AVehicleBase::AVehicleBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVehicleBase::BeginPlay()
{
	Super::BeginPlay();
	
}


void AVehicleBase::UpdateLocationFromVelocity(FVector Delta)
{
	FHitResult HitResult;
	
	AddActorWorldOffset(Delta, true, &HitResult);

	if(HitResult.bBlockingHit)
	{
		Velocity = FVector::ZeroVector;
	}
}

void AVehicleBase::ApplyRotation(float DeltaTime)
{
	float RotationAngle = MaxDegreePersecond * DeltaTime * SteeringThrow;

	FQuat RotationDelta(GetActorUpVector(), FMath::DegreesToRadians(RotationAngle));

	Velocity = RotationDelta.RotateVector(Velocity);

	AddActorWorldRotation(RotationDelta);
}

// Called every frame
void AVehicleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;
	FVector Acceleration = Force / Mass;

	Velocity = Velocity + Acceleration * DeltaTime;

	FVector Delta = Velocity * 100 * DeltaTime;

	UpdateLocationFromVelocity(Delta);

	ApplyRotation(DeltaTime);
}

// Called to bind functionality to input
void AVehicleBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AVehicleBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVehicleBase::MoveRight);
}

void AVehicleBase::MoveForward(float Value)
{
	Throttle = Value;
}

void AVehicleBase::MoveRight(float Value)
{
	SteeringThrow = Value;
}

