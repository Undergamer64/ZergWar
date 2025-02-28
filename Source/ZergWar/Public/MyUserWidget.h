// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class ZERGWAR_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void DrawBox(bool draw, FVector2d start, FVector2d end);

	FVector ScreenToWorld(const APlayerController* controller, FVector2d point) const;
};
