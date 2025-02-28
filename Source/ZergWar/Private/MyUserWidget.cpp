// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"

#include <Programs/UnrealBuildAccelerator/Core/Public/UbaBase.h>

void UMyUserWidget::DrawBox(bool draw, FVector2d start, FVector2d end)
{
	if (draw)
	{
		FVector2D Result = FVector2D(1920, 1080);

		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize( Result);
		}
		
		SetVisibility(ESlateVisibility::Visible);

		FVector2d scale = FVector2D(abs(start.X - end.X) / Result.X, abs(start.Y - end.Y) / Result.Y);
		SetRenderScale(scale);
		
		FVector2d pos = FVector2D(uba::Min( start.X, end.X) + scale.X, uba::Min(start.Y, end.Y) + scale.Y);
		SetPositionInViewport(pos);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}
