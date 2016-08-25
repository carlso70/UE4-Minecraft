// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "MinecraftCharacter.generated.h"

class UInputComponent;
class AWieldable;
 
UCLASS(config=Game)
class AMinecraftCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;
	
public:
	AMinecraftCharacter();

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_WieldedItem;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AMinecraftProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/* ----------------------------Inventory and HUD Items --------------------------*/
	/* Get the current inveotrory slot for the HUD */
	UFUNCTION(BlueprintPure, Category = HUD)
	int32 GetCurrentInventorySlot();

	/* Adds item to the inventory */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool AddItemToInventory(AWieldable* Item);

	/* Gets the thumbnail for a given item */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UTexture2D* GetThumbnailAtInventorySlot(uint8 Slot);

	/* Type of tool and tool material of current wielded item */
	uint8 ToolType;
	uint8 MaterialType;

protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

private:

	/* True if player is breaking block, false otherwise */
	bool bIsBreaking;

	/* Fire when hitting with a tool */
	void OnHit();
	void EndHit();

	/* Plays the mining animation */
	void PlayHitAnim();

	/* Check if there is a block in front of the player*/
	void CheckForBlocks();

	/* Called when we want to break a block */
	void BreakBlock();

	/* Stores the block currently being looked at by the player */
	UPROPERTY()
	class ABlock* CurrentBlock;

	/* The character's reach */
	float Reach;

	/* Time between breaking blocks */
	FTimerHandle BlockBreakingHandle;
	/* Times between pickaxe swings */
	FTimerHandle HitAnimHandle;



	/* ---------------------------------Player's Inventory--------------------------*/

	/* Player's Inventory */
	UPROPERTY(EditAnywhere)
	TArray<AWieldable*> Inventory;

	/* Current Inventory Slot */
	int32 CurrentInventorySlot;

	/* Updates the wielded item */
	void UpdateWieldedItem();

	/* Gets the currently wielded Item */
	AWieldable* GetCurrentWieldedItem();

	/* Throws Currently Wielded Item*/
	void Throw();

	/* Number of inventory slots */
	const int32 NUM_OF_INVENTORY_SLOTS = 10;

	/* Change inventory slot */
	void MoveUpInventorySlot();
	void MoveDownInventorySlot();

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

