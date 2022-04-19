// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinecraftGameMode.h"
#include "MinecraftHUD.h"
#include "MinecraftCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMinecraftGameMode::AMinecraftGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMinecraftHUD::StaticClass();

	HUDState = EHUDState::HS_Ingame;
}

void AMinecraftGameMode::BeginPlay() {
	Super::BeginPlay();

	ApplyHUDChanges();
}


void AMinecraftGameMode::ApplyHUDChanges() {
	if (CurrentWidget != nullptr) {
		CurrentWidget->RemoveFromParent();
	}

	switch(HUDState) {
		case EHUDState::HS_Ingame: {
			ApplyHUD(IngameHUDClass, false, false);
		}
		case EHUDState::HS_Inventory: {
			ApplyHUD(InventoryHUDClass, true, true);
		}
		case EHUDState::HS_CraftMenu: {
			ApplyHUD(CraftMenuHUDClass, true, true);
		}
		default: {
			ApplyHUD(IngameHUDClass, false, false);
		}
	}
}

TEnumAsByte<EHUDState> AMinecraftGameMode::GetHUDState() const {
	return HUDState;
}

void AMinecraftGameMode::ChangeHUDState(TEnumAsByte<EHUDState> NewState) {
	HUDState = NewState;
	ApplyHUDChanges();
}

bool AMinecraftGameMode::ApplyHUD(TSubclassOf<class UUserWidget> WidgetToApply, bool ShowMouseCursor, bool EnableClickEvents) {
	APlayerController* MyController = GetWorld()->GetFirstPlayerController();

	if (WidgetToApply != nullptr) {
		MyController->bShowMouseCursor = ShowMouseCursor;
		MyController->bEnableClickEvents = EnableClickEvents;

		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetToApply);

		if (CurrentWidget != nullptr) {
			CurrentWidget->AddToViewport(0);
			return true;
		}
	}

	return false;
}
