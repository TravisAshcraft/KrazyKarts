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
	UPROPERTY()
	float TimeStamp;
	
};

USTRUCT()
struct FVehicleState
{
	GENERATED_BODY()

	UPROPERTY()
	FVehicleMove LastMove;

	UPROPERTY()
	FTransform VehicleTransform;

	UPROPERTY()
	FVector Velocity;
	
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
	void SimulateMove(FVehicleMove Move);
	
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
	
	FVector Velocity;
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FVehicleMove Move);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FVehicleState ServerState;

	UFUNCTION()
	void OnRep_ServerState();

};
