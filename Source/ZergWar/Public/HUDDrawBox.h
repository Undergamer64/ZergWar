// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDDrawBox.generated.h"

/**
 * 
 */
UINTERFACE()
class ZERGWAR_API UHUDDrawBox : public UInterface
{
	GENERATED_BODY()
};

class ZERGWAR_API IHUDDrawBox
{
	GENERATED_BODY()

public:
	virtual void DrawBox(bool draw, FVector2D start, FVector2D end);
};
