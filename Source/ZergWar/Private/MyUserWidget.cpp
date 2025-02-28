// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"

#include <Programs/UnrealBuildAccelerator/Core/Public/UbaBase.h>

#include "BasicMinion.h"
#include "FrameTypes.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/GameSession.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UMyUserWidget::DrawBox(bool draw, FVector2d start, FVector2d end)
{
	if (draw)
	{
		SetVisibility(ESlateVisibility::Visible);

		FVector2d size = FVector2D(abs(start.X - end.X), abs(start.Y - end.Y));
		FVector2D pos = FVector2D(FMath::Min(start.X, end.X), FMath::Min(start.Y, end.Y));

		SetAlignmentInViewport(FVector2D(0, 0));  // Ensure no unexpected shifting
		SetPositionInViewport(pos, false);
		SetDesiredSizeInViewport(size);

		//Sorry, the tutorial did this and it's horrible, i was supposed to do that twice !
		/*
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

		if (!PlayerController) return;

		FVector2D UpLeft = FVector2D(uba::Min(start.X, end.X), uba::Min(start.X, end.X));
		FVector2D UpRight = FVector2D(uba::Min(start.X, end.X), uba::Max(start.X, end.X));
		FVector2D DownLeft = FVector2D(uba::Max(start.X, end.X), uba::Min(start.X, end.X));
		FVector2D DownRight = FVector2D(uba::Max(start.X, end.X), uba::Max(start.X, end.X));

		FVector WP1 = ScreenToWorld(PlayerController, UpLeft);
		FVector WP2 = ScreenToWorld(PlayerController, UpRight);
		FVector WP3 = ScreenToWorld(PlayerController, DownLeft);
		FVector WP4 = ScreenToWorld(PlayerController, DownRight);

		TArray<AActor*> ActorsToIgnore;
		TArray<FHitResult> Pawns;
		
		UKismetSystemLibrary::BoxTraceMulti(
			GetWorld(),
			FMath::Lerp(WP1, WP3, 0.5f),
			FMath::Lerp(WP2, WP4, 0.5f) + (WP4-WP3),
				FVector(uba::Min(
					FVector::Distance(WP1, WP3),
					FVector::Distance(WP1, WP3)
					) * 0.5f, 10, 100),
			UKismetMathLibrary::FindLookAtRotation(WP1, WP3),
			TraceTypeQuery_MAX,
			true,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			Pawns,
			true
			);
		*/
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

FVector UMyUserWidget::ScreenToWorld(const APlayerController* controller, FVector2D point) const
{
	FVector Result = FVector(0, 0, 0);
	FVector Dir = FVector(0, 0, 0);

	controller->DeprojectScreenPositionToWorld(point.X, point.Y, Result, Dir);
	FHitResult OutHit = FHitResult();

	bool success = GetWorld()->LineTraceSingleByChannel(OutHit, Result, Result + Dir*1000000, ECC_Visibility);

	if (success) return OutHit.Location;
	
	return FVector::Zero();
}
