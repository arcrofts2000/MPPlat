// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MPPGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class COOPADVENTURE_API UMPPGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UMPPGameInstanceSubsystem();
	
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	void PrintString(const FString& InString);
};
