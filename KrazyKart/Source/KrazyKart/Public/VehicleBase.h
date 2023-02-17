// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VehicleBase.generated.h"

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
	void UpdateLocationFromVelocity(FVector Delta);
	void ApplyRotation(float DeltaTime);

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

	// The number of degrees rotated per second
	UPROPERTY(EditAnywhere)
	float MaxDegreePersecond = 90;

	float Throttle;
	float SteeringThrow;

	FVector Velocity;

	

	void MoveForward(float Value);
	void MoveRight(float Value);

};
