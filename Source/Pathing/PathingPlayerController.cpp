// Copyright Epic Games, Inc. All Rights Reserved.

#include "PathingPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "PathingCharacter.h"
#include "Engine/World.h"

APathingPlayerController::APathingPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void APathingPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void APathingPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &APathingPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &APathingPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APathingPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &APathingPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &APathingPlayerController::OnResetVR);
}

void APathingPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void APathingPlayerController::MoveToMouseCursor()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (APathingCharacter* MyPawn = Cast<APathingCharacter>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}
}

void APathingPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void APathingPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void APathingPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void APathingPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}
