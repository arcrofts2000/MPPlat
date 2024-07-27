// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MPPGameInstanceSubsystem.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"

UMPPGameInstanceSubsystem::UMPPGameInstanceSubsystem()
{
	//PrintString("MSS Constructor");
	bCreateServerAfterDestroy = false;
	DestroyedServerName = "";
	ServerNameToFind = "";
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
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMPPGameInstanceSubsystem::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMPPGameInstanceSubsystem::OnFindSessionsComplete);
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
		bCreateServerAfterDestroy = true;
		DestroyedServerName = ServerName;
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
	SessionSettings.Set(FName("SERVER_NAME"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(0, SessionName, SessionSettings);
}

void UMPPGameInstanceSubsystem::FindServer(FString ServerName)
{
	PrintString("FindServer");

	if (ServerName.IsEmpty())
	{
		PrintString("Server name cannot be empty!");
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	bool bIsLAN = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		bIsLAN = true;

	SessionSearch->bIsLanQuery = bIsLAN;
	SessionSearch->MaxSearchResults = 9999;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	ServerNameToFind = ServerName;

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMPPGameInstanceSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), bWasSuccessful));

	if (bWasSuccessful)
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
	}
}

void UMPPGameInstanceSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	PrintString(FString::Printf(TEXT("OnDestroySessionComplete: %d"), bWasSuccessful));

	if (bCreateServerAfterDestroy)
	{
		bCreateServerAfterDestroy = false;
		CreateServer(DestroyedServerName);
	}
}

void UMPPGameInstanceSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful)
		return;

	if (ServerNameToFind.IsEmpty())
		return;

	TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
	if (Results.Num() > 0)
	{
		PrintString(FString::Printf(TEXT("%d Sessions found."), Results.Num()));
		for (FOnlineSessionSearchResult Result : Results)
		{
			if (Result.IsValid())
			{
				FString ServerName = "No-Name";
				Result.Session.SessionSettings.Get(FName("SERVER_NAME"), ServerName);

				PrintString(FString::Printf(TEXT("ServerName: %s"), *ServerName));
			}
		}
	}
	else
	{
		PrintString("Zero sessions found.");
	}
}