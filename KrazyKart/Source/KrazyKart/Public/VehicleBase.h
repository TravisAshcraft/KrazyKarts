// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Vehicle/VehicleMovementComponent.h"
#include "VehicleBase.generated.h"


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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void ClearAcknowledeMoves(FVehicleMove VehicleMove);
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FVehicleMove Move);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FVehicleState ServerState;

	UFUNCTION()
	void OnRep_ServerState();

	TArray<FVehicleMove> UnacknowledgeMoves;

	UPROPERTY(EditAnywhere)
	UVehicleMovementComponent* MovementComponent;

};
