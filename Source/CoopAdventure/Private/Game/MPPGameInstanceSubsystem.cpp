// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MPPGameInstanceSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

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
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMPPGameInstanceSubsystem::OnCreateSessionComplete);
			
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

void UMPPGameInstanceSubsystem::CreateServer(FString ServerName)
{
	PrintString("CreateServer");

	if (ServerName.IsEmpty())
	{
		PrintString("Server name cannot be empty!");
		return;
	}

	FName SessionName = FName("Co-op Adventure Session Name");

	if (FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SessionName))
	{
		PrintString(FString::Printf(TEXT("Session with name %s already exists, destroying it."), *SessionName.ToString()));
		SessionInterface->DestroySession(SessionName);
		return;
	}
	
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;

	bool bIsLan = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		bIsLan = true;

	SessionSettings.bIsLANMatch = bIsLan;

	SessionInterface->CreateSession(0, SessionName, SessionSettings);
}

void UMPPGameInstanceSubsystem::FindServer(FString ServerName)
{
	PrintString("FindServer");
}

void UMPPGameInstanceSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), bWasSuccessful));

	if (bWasSuccessful)
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
	}
}