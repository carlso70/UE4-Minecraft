// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "MinecraftGameMode.generated.h"

UCLASS(minimalapi)
class AMinecraftGameMode : public AGameMode
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
	AMinecraftGameMode();

	enum EHUDState : uint8
	{
		HS_Ingame,
		HS_Inventory,
		HS_Craft_Menu,
	};

	/* Check HUD state, then calls ApplyHUD to apply whatever hud we are using*/
	void ApplyHUDChanges();

	/* Gets the HUD State */
	uint8 GetHUDState();
		
	/* Sets new HUD state then calls ApplyHUDChanges() */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ChangeHUDState(uint8 NewState);

	/* Applies HUD to screen, true if successful, false otherwise */
	bool ApplyHUD(TSubclassOf<class UUserWidget> WidgetToApply, bool ShowMouseCursor, bool EnableClickEvents);

protected:
	
	/* HUD to be shown in game */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Blueprint Widgets", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> IngameHUDClass;

	/* HUD to be shown in the players inventory */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Blueprint Widgets", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> InventoryHUDClass;

	/* HUD to be shown in crafting menu */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Blueprint Widgets", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> CraftMenuHUDClass;

	/* Current HUD state*/
	uint8 HUDState;

	/* Current HUD being displayed*/
	class UUserWidget* CurrentWidget;

};



