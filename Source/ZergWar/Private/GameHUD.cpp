// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AAAAAAAAAAAAAAAAAAAH"));
	
	BoxWidget = Cast<UMyUserWidget>(CreateWidget<UMyUserWidget>(GetOwningPlayerController(), BoxWidgetTemplate));
	if (BoxWidget != nullptr)
	{
		BoxWidget->AddToViewport();
	}
}

void AGameHUD::DrawBox(bool draw, FVector2D start, FVector2D end)
{
	if (BoxWidget == nullptr) return;
	
	BoxWidget->DrawBox(draw, start, end);
}