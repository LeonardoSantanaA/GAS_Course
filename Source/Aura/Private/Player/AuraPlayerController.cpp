// Copyright Lekaum


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/HighlightInterface.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Crashes the game if AuraContext doesnt valid
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);

	Subsystem->AddMappingContext(AuraContext, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// castchecked to crash if fails
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}


void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	if (!CursorHit.bBlockingHit) return;

	// update last highlight actor to current highlight actor
	LastHighlightActor = ThisHighlightActor;

	// update this highlight actor if actor from HitResult implements HighlightInterface (thanks to TScriptInterface verifying it automatically)
	ThisHighlightActor = CursorHit.GetActor();

	/**
	 * Line trace from cursor. There are serveral scenarios:
	 * A. LastHighlightActor is null && ThisHightlightActor is null
	 *		- Do nothing
	 * B. LastHighlightActor is null && ThisHightlightActor is valid
	 *		- Highlight ThisHightlightActor
	 * C. LastHighlightActor is valid && ThisHightlightActor is null
	 *		- UnHighlight LastHighlightActor
	 * D. Both actors are valid, but LastHighlightActor != ThisHightlightActor
	 *		- UnHighlight LastHighlightActor, and Hightlight ThisHightlightActor
	 * E. Both actors are valid, but are the same actor
	 *		- Do nothing
	 **/
	if (LastHighlightActor == nullptr)
	{
		if (ThisHighlightActor != nullptr)
		{
			// Case B
			ThisHighlightActor->HighlightActor();
		}
		else
		{
			// Case A - both are null,  do nothing
		}
	}
	// LastHighlightActor is valid
	else
	{
		if (ThisHighlightActor == nullptr)
		{
			// Case C
			LastHighlightActor->UnHighlightActor();
		}
		// Both Actors are valid 
		else
		{
			if (LastHighlightActor != ThisHighlightActor)
			{
				// Case D
				LastHighlightActor->UnHighlightActor();
				ThisHighlightActor->HighlightActor();
			}
			else
			{
				// Case E - do nothing
			}
		}
	}
}
