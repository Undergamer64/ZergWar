// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZergWarPlayerController.h"

#include "BasicMinion.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "ZergWarCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "HUDDrawBox.h"
#include "VectorUtil.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/HUD.h"
#include "GameFramework/SpringArmComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AZergWarPlayerController::AZergWarPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AZergWarPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	SelectedMinions.Init(nullptr, 0);
}

void AZergWarPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(CancelClickAction, ETriggerEvent::Started, this, &AZergWarPlayerController::OnCancel);
		
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AZergWarPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AZergWarPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AZergWarPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AZergWarPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AZergWarPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AZergWarPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AZergWarPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AZergWarPlayerController::OnTouchReleased);
  
		// Setup Move
		EnhancedInputComponent->BindAction(MoveCameraAction, ETriggerEvent::Triggered, this, &AZergWarPlayerController::OnMoveCamera);
		EnhancedInputComponent->BindAction(ZoomCameraAction, ETriggerEvent::Triggered, this, &AZergWarPlayerController::OnZoomCamera);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AZergWarPlayerController::OnInputStarted()
{
	FollowTime = GetWorld()->GetTime().GetRealTimeSeconds();
	
	StartMousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
}

void AZergWarPlayerController::OnCancel()
{
	bSelectClick = false;
	/*
	if (CurrentMinion != nullptr)
	{
		SelectMinion(false);
	}
	*/

	if (SelectedMinions.Num() > 0)
	{
		for (ABasicMinion* Minion : SelectedMinions)
		{
			SelectMinion(Minion, false);
			SelectedMinions.Remove(Minion);
		}
	}
}

void AZergWarPlayerController::SelectMinion(ABasicMinion* Minion, bool selected)
{
	if (Minion != nullptr)
	{
		bSelectClick = selected;
		
		UStaticMeshComponent* cylinder = static_cast<UStaticMeshComponent*>(
			Minion->GetComponentByClass(
				UStaticMeshComponent::StaticClass()
			)
		);
		
		if (cylinder != nullptr) cylinder->SetVisibility(selected);
	}
}

// Triggered every frame when the input is held down
void AZergWarPlayerController::OnSetDestinationTriggered()
{
	if (GetWorld()->GetTime().GetRealTimeSeconds() - FollowTime >= 0.1f)
	{
		FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());

		AHUD* hud = GetHUD();

		if (hud->Implements<UHUDDrawBox>())
		{
			Cast<IHUDDrawBox>(hud)->DrawBox(true, StartMousePos, MousePos);
		}
	}
}

void AZergWarPlayerController::OnSetDestinationReleased()
{
	if (GetWorld()->GetTime().GetRealTimeSeconds() - FollowTime >= 0.1f)
	{
		AHUD* hud = GetHUD();

		if (hud->Implements<UHUDDrawBox>())
		{
			Cast<IHUDDrawBox>(hud)->DrawBox(false, FVector2D::Zero(), FVector2d::Zero());
		}
		
		return;
	}

	FHitResult Hit;
	if (GetHitResultUnderCursor(ECollisionChannel::ECC_MAX, true, Hit))
	{
		if (Hit.GetActor()->IsA(ABasicMinion::StaticClass()))
		{
			ABasicMinion* Minion = static_cast<ABasicMinion*>(Hit.GetActor());
			SelectedMinions.Add(Minion);
			bSelectClick = true;
			SelectMinion(Minion, true);
		}
		CachedDestination = Hit.Location;
	}
	
	if (SelectedMinions.Num() > 0)
	{
		if (bSelectClick)
		{
			bSelectClick = false;
			return;
		}
		
		// We move there and spawn some particles
		//UAIBlueprintHelperLibrary::SimpleMoveToLocation(CurrentMinion->GetController(), CachedDestination);

		for (ABasicMinion* Minion : SelectedMinions)
		{
			if (Minion == nullptr)
			{
				SelectedMinions.Remove(Minion);
				continue;
			}
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Minion->GetController(), CachedDestination);
		}
		
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}
}

// Triggered every frame when the input is held down
void AZergWarPlayerController::OnTouchTriggered()
{
	OnSetDestinationTriggered();
}

void AZergWarPlayerController::OnTouchReleased()
{
	OnSetDestinationReleased();
}

void AZergWarPlayerController::OnMoveCamera(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	APawn* player = GetPawn();
	
	if (player == nullptr)
	{
		return;
	}

	const FVector vector(Input * CameraSpeed, 0.0);
	player->SetActorLocation(player->GetActorLocation() + vector);
}

void AZergWarPlayerController::OnZoomCamera(const FInputActionValue& Value)
{
	const float Input = Value.Get<float>() * 50;
	APawn* player = GetPawn();
	
	if (player == nullptr)
	{
		return;
	}

	USpringArmComponent* cameraBoom = static_cast<USpringArmComponent*>(player->GetComponentByClass(USpringArmComponent::StaticClass()));

	cameraBoom->TargetArmLength = UE::Geometry::VectorUtil::Clamp(cameraBoom->TargetArmLength - Input, 100.f, 1325.f);
}
