// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleMovementComponent.generated.h"

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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKART_API UVehicleMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVehicleMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SimulateMove(FVehicleMove Move);

	FVehicleMove Createmove(float DeltaTime);

	FVector GetVelocity() {return Velocity;}
	void SetVelocity(FVector Value) {Velocity = Value;}

	void SetSteeringThrow(float Value) {SteeringThrow = Value;}
	void SetThrottle(float Value) {Throttle = Value;}
	

private:
	void UpdateLocationFromVelocity(float DeltaTime);
	void ApplyRotation(float DeltaTime, float Steering);
	FVector GetDragResistance();
	FVector GetRollingResistance();

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

	FVector Velocity;

	float Throttle;
	float SteeringThrow;
};
