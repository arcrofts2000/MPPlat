// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
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

	IOnlineSessionPtr SessionInterface;

	UFUNCTION(BlueprintCallable, Category = "Online")
	void CreateServer(FString ServerName);

	UFUNCTION(BlueprintCallable, Category = "Online")
	void FindServer(FString ServerName);

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);

	bool bCreateServerAfterDestroy;
	FString DestroyedServerName;
	FString ServerNameToFind;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
};