// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Minecraft.h"
#include "MinecraftGameMode.h"
#include "MinecraftHUD.h"
#include "MinecraftCharacter.h"
#include "MinecraftCharacter.h"
#include "Blueprint/UserWidget.h"

AMinecraftGameMode::AMinecraftGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMinecraftHUD::StaticClass();
}

void AMinecraftGameMode::BeginPlay()
{
	Super::BeginPlay();

	/* Immedietly on game start apply HUD */
	ApplyHUDChanges();
}

void AMinecraftGameMode::ApplyHUDChanges()
{
	/* Remove previous HUD */
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromParent();
	}

	/* Check HUD state and apply correct HUD */
	switch (HUDState)
	{
		case EHUDState::HS_Ingame:
		{
			ApplyHUD(IngameHUDClass, false, false);
		}
		case EHUDState::HS_Inventory:
		{
			ApplyHUD(InventoryHUDClass, true, true);
		}
		case EHUDState::HS_Craft_Menu:
		{
			ApplyHUD(CraftMenuHUDClass, true, true);
		}
		default:
		{
			ApplyHUD(IngameHUDClass, false, false);
		}
	}
}

uint8 AMinecraftGameMode::GetHUDState()
{
	return HUDState;
}

void AMinecraftGameMode::ChangeHUDState(uint8 NewState)
{
	HUDState = NewState;
	ApplyHUDChanges();
}

bool AMinecraftGameMode::ApplyHUD(TSubclassOf<class UUserWidget> WidgetToApply, bool ShowMouseCursor, bool EnableClickEvents)
{
	/* Get reference to player and player controller */
	AMinecraftCharacter* Character = Cast<AMinecraftCharacter>(UGameplayStatics::GetPlayerController(this, 0));
	APlayerController* Controller = GetWorld()->GetFirstPlayerController();

	if (WidgetToApply != nullptr)
	{
		/* Set mouse events and visibliity according to the parameters taken by the function */
		Controller->bEnableClickEvents = EnableClickEvents;
		Controller->bShowMouseCursor = ShowMouseCursor;

		/* Create the widget */
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetToApply);
		
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
			return true;
		}
	}
	return false;
}
