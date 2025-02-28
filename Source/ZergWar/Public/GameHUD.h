// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDDrawBox.h"
#include "MyUserWidget.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class ZERGWAR_API AGameHUD : public AHUD, public IHUDDrawBox
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> BoxWidgetTemplate;

	virtual void DrawBox(bool draw, FVector2D start, FVector2D end) override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	UMyUserWidget* BoxWidget;
};
