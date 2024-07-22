// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MPPGameInstanceSubsystem.h"
#include "OnlineSubsystem.h"

UMPPGameInstanceSubsystem::UMPPGameInstanceSubsystem()
{
	//PrintString("MSS Constructor");
}

void UMPPGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//PrintString("MSS Initialize");

	if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		FString SubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
		PrintString(SubsystemName);

		SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			PrintString("SessionInterface is Valid.");
			
		}
	}
}

void UMPPGameInstanceSubsystem::Deinitialize()
{
	//UE_LOG(LogTemp, Warning, TEXT("MSS Deinitialize"));
}

void UMPPGameInstanceSubsystem::PrintString(const FString& InString)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, InString);
}