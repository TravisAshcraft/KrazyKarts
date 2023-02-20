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

	MovementComponent = CreateDefaultSubobject<UVehicleMovementComponent>(TEXT("MovementComp"));
}

// Called when the game starts or when spawned
void AVehicleBase::BeginPlay()
{
	Super::BeginPlay();
}

void AVehicleBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AVehicleBase, ServerState);
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

// Called every frame
void AVehicleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(MovementComponent == nullptr)
	{
		return;
	}
	
	if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		FVehicleMove Move = MovementComponent->Createmove(DeltaTime);
		MovementComponent->SimulateMove(Move);

		UnacknowledgeMoves.Add(Move);
		Server_SendMove(Move);
	}
	if(GetLocalRole() == ROLE_Authority && IsLocallyControlled())
	{
		FVehicleMove Move = MovementComponent->Createmove(DeltaTime);
		Server_SendMove(Move);
	}

	DrawDebugString(GetWorld(), FVector(0,0, 100), GetEnumText(GetLocalRole()), this, FColor::White, DeltaTime);

	if(GetLocalRole() == ROLE_SimulatedProxy)
	{
		MovementComponent->SimulateMove(ServerState.LastMove);
	}
	
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


void AVehicleBase::ClearAcknowledeMoves(FVehicleMove LastMove)
{
	TArray<FVehicleMove> NewMoves;

	for (const FVehicleMove& Move : UnacknowledgeMoves)
	{
		if(Move.TimeStamp > LastMove.TimeStamp)
		{
			//We check the timestamp of the new move
			//If it is greater than the last then we need to add it
			NewMoves.Add(Move);
		}
	}

	//We set the unacknowledgeMoves to NewMoves
	UnacknowledgeMoves = NewMoves;
}

void AVehicleBase::MoveForward(float Value)
{
	if(MovementComponent == nullptr)
	{
		return;
	}
	MovementComponent->SetThrottle(Value);
}

void AVehicleBase::MoveRight(float Value)
{
	if(MovementComponent == nullptr)
	{
		return;
	}
	MovementComponent->SetSteeringThrow(Value);
}

void AVehicleBase::OnRep_ServerState()
{

	if(MovementComponent == nullptr)
	{
		return;
	}
	
	SetActorTransform(ServerState.VehicleTransform);
	MovementComponent->SetVelocity(ServerState.Velocity);

	ClearAcknowledeMoves(ServerState.LastMove);

	for (const FVehicleMove& Move : UnacknowledgeMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
}

void AVehicleBase::Server_SendMove_Implementation(FVehicleMove Move)
{
	if(MovementComponent == nullptr)
	{
		return;
	}
	MovementComponent->SimulateMove(Move);

	ServerState.LastMove = Move;
	ServerState.VehicleTransform = GetActorTransform();
	ServerState.Velocity = MovementComponent->GetVelocity();
}

bool AVehicleBase::Server_SendMove_Validate(FVehicleMove Move)
{
	return true;
}
