// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MinecraftGameMode.generated.h"

UENUM()
enum EHUDState {
	HS_Ingame,
	HS_Inventory,
	HS_CraftMenu
};

UCLASS(minimalapi)
class AMinecraftGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMinecraftGameMode();

	TEnumAsByte<EHUDState> GetHUDState();

	void ApplyHUDChanges();

	UFUNCTION(BlueprintCallable, Category = "HUD Functions")
	void ChangeHUDState(TEnumAsByte<EHUDState> NewState);

	bool ApplyHUD(TSubclassOf<class UUserWidget> WidgetToApply, bool ShowMouseCursor, bool EnableClickEvents);

protected:
	virtual void BeginPlay() override;

	TEnumAsByte<EHUDState>  HUDState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Blueprint Widgets", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> IngameHUDClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Blueprint Widgets", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> InventoryHUDClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Blueprint Widgets", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> CraftMenuHUDClass;

	class UUserWidget* CurrentWidget;

};



