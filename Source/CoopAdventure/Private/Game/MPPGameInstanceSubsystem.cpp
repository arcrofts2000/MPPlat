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
	MySessionName = FName("Co-op Adventure Session Name");
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
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMPPGameInstanceSubsystem::OnJoinSessionComplete);
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

	if (FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(MySessionName))
	{
		PrintString(FString::Printf(TEXT("Session with name %s already exists, destroying it."), *MySessionName.ToString()));
		bCreateServerAfterDestroy = true;
		DestroyedServerName = ServerName;
		SessionInterface->DestroySession(MySessionName);
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

	SessionInterface->CreateSession(0, MySessionName, SessionSettings);
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
	FOnlineSessionSearchResult* CorrectResult = 0;

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

				if (ServerName.Equals(ServerNameToFind))
				{
					CorrectResult = &Result;
					PrintString(FString::Printf(TEXT("Found Server with name: %s"), *ServerName));
					break;
				}
			}
		}
		if (CorrectResult)
		{
			SessionInterface->JoinSession(0, MySessionName, *CorrectResult);
		}
		else
		{
			PrintString(FString::Printf(TEXT("Couldn't find server: %s"), *ServerNameToFind));
			ServerNameToFind = 0;
		}
	}
	else
	{
		PrintString("Zero sessions found.");
	}
}

void UMPPGameInstanceSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		PrintString(FString::Printf(TEXT("Successfully joined session: %s"), *SessionName.ToString()));

		FString Address = "";
		if (bool bSuccess = SessionInterface->GetResolvedConnectString(MySessionName, Address))
		{
			PrintString(FString::Printf(TEXT("Address: %s"), *Address));
			if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
				PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
		else
			PrintString("GetResolvedConnectString returned false!");
	}
	else
		PrintString("OnJoinSessionComplete failed.");
}