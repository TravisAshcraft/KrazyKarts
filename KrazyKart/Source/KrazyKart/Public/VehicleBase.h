// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VehicleBase.generated.h"

USTRUCT()
struct FVehicleMove
{
	GENERATED_BODY()

	UPROPERTY()
	float Throttle;
	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float DeltaTime;
	
};

USTRUCT()
struct FVehicleState
{
	GENERATED_BODY()

	UPROPERTY()
	FVehicleMove LastMove;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;
	
};

UCLASS()
class KRAZYKART_API AVehicleBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVehicleBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void UpdateLocationFromVelocity(float DeltaTime);
	void ApplyRotation(float DeltaTime);
	FVector GetDragResistance();
	FVector GetRollingResistance();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	// The force applied to the car when the throttle is fully down (N).
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	// The min Turn radius at full lock (m).
	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10;

	// The amount of drag on this vehicle
	// Higher the number the more resistance.
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	// The amount of Rolling Resistance on this vehicle
	// Higher the number the more resistance.
	UPROPERTY(EditAnywhere)
	float RollingResistance = 0.015;

	UPROPERTY(Replicated)
	float Throttle;
	UPROPERTY(Replicated)
	float SteeringThrow;

	UPROPERTY(Replicated)
	FVector Velocity;

	UPROPERTY(ReplicatedUsing= OnRep_ReplicatedLocation)
	FVector ReplicatedLocation;

	//OnReps have to be flagged as UFunction()
	UFUNCTION() 
	void OnRep_ReplicatedLocation();

	UPROPERTY(Replicated)
	FRotator ReplcatedRotation;

	void MoveForward(float Value);
	void MoveRight(float Value);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);

};
