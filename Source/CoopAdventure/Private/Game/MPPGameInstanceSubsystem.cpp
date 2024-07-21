// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MPPGameInstanceSubsystem.h"

UMPPGameInstanceSubsystem::UMPPGameInstanceSubsystem()
{
	PrintString("MSS Constructor");
}

void UMPPGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	PrintString("MSS Initialize");
}

void UMPPGameInstanceSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("MSS Deinitialize"));
}

void UMPPGameInstanceSubsystem::PrintString(const FString& InString)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, InString);
}