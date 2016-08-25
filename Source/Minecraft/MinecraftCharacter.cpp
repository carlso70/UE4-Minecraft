// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Minecraft.h"
#include "Block.h"
#include "MinecraftCharacter.h"
#include "MinecraftProjectile.h"
#include "Wieldable.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMinecraftCharacter

AMinecraftCharacter::AMinecraftCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_WieldedItem = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_WieldedItem"));
	FP_WieldedItem->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_WieldedItem->bCastDynamicShadow = false;
	FP_WieldedItem->CastShadow = false;
	FP_WieldedItem->AttachTo(Mesh1P, TEXT("GripPoint"), EAttachLocation::SnapToTargetIncludingScale, true);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->AttachTo(FP_WieldedItem);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	/* Character's arm reach */
	Reach = 250.f;

	Inventory.SetNum(NUM_OF_INVENTORY_SLOTS);
}

void AMinecraftCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForBlocks();
}

void AMinecraftCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	FP_WieldedItem->AttachTo(Mesh1P, TEXT("GripPoint"), EAttachLocation::SnapToTargetIncludingScale, true); //Attach gun mesh component to Skeleton, doing it here because the skelton is not yet created in the constructor
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMinecraftCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("InventoryUp", IE_Pressed, this, &AMinecraftCharacter::MoveUpInventorySlot);
	InputComponent->BindAction("InventoryDown", IE_Pressed, this, &AMinecraftCharacter::MoveDownInventorySlot);

	InputComponent->BindAction("Throw", IE_Pressed, this, &AMinecraftCharacter::Throw);

	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMinecraftCharacter::TouchStarted);
	if (EnableTouchscreenMovement(InputComponent) == false)
	{
		InputComponent->BindAction("Interact", IE_Pressed, this, &AMinecraftCharacter::OnHit);
		InputComponent->BindAction("Interact", IE_Released, this, &AMinecraftCharacter::EndHit);
	}

	InputComponent->BindAxis("MoveForward", this, &AMinecraftCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMinecraftCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AMinecraftCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AMinecraftCharacter::LookUpAtRate);
}

int32 AMinecraftCharacter::GetCurrentInventorySlot()
{
	return CurrentInventorySlot;
}

bool AMinecraftCharacter::AddItemToInventory(AWieldable * Item)
{
	if (Item)
	{
		const int32 AvailableSlot = Inventory.Find(nullptr);
		
		if (AvailableSlot != INDEX_NONE)
		{
			Inventory[AvailableSlot] = Item;
			return true;
		}
	}
	return false;
}

UTexture2D * AMinecraftCharacter::GetThumbnailAtInventorySlot(uint8 Slot)
{
	if (Inventory[Slot])
	{
		return Inventory[Slot]->PickupThumbnail;
	}
	return nullptr;
}

void AMinecraftCharacter::OnFire()
{
}

void AMinecraftCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AMinecraftCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

void AMinecraftCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y * BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void AMinecraftCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMinecraftCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMinecraftCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMinecraftCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AMinecraftCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMinecraftCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &AMinecraftCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AMinecraftCharacter::TouchUpdate);
	}
	return bResult;
}

void AMinecraftCharacter::CheckForBlocks()
{
	FHitResult LineTraceHit;

	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
	FVector EndTrace = (FirstPersonCameraComponent->GetForwardVector() * Reach) + StartTrace;

	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(LineTraceHit, StartTrace, EndTrace, ECollisionChannel::ECC_WorldDynamic, CQP);

	ABlock* PotentialBlock = Cast<ABlock>(LineTraceHit.GetActor());

	if (PotentialBlock != CurrentBlock && CurrentBlock != nullptr)
	{
		CurrentBlock->ResetBlock();
	}

	if (PotentialBlock == nullptr)
	{
		CurrentBlock = nullptr;
		return;
	}
	else
	{
		if (CurrentBlock != nullptr && !bIsBreaking)
		{
			CurrentBlock->ResetBlock();
		}
		CurrentBlock = PotentialBlock;
	}
}

void AMinecraftCharacter::PlayHitAnim()
{
	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AMinecraftCharacter::OnHit()
{
	PlayHitAnim();

	if (CurrentBlock != nullptr)
	{
		bIsBreaking = true;

		float TimeBetweenBreaks = ((CurrentBlock->Resistance) / 100.f) / 2.f;

		GetWorld()->GetTimerManager().SetTimer(BlockBreakingHandle, this, &AMinecraftCharacter::BreakBlock, TimeBetweenBreaks, true);
		// 0.4f the time between pickaxe swings, TODO change that to whatever type of axe we have wielded's speed
		GetWorld()->GetTimerManager().SetTimer(HitAnimHandle, this, &AMinecraftCharacter::PlayHitAnim, 0.4f, true);
	}
}

void AMinecraftCharacter::EndHit()
{
	GetWorld()->GetTimerManager().ClearTimer(BlockBreakingHandle);
	GetWorld()->GetTimerManager().ClearTimer(HitAnimHandle);

	bIsBreaking = false;

	if (CurrentBlock != nullptr)
	{
		CurrentBlock->ResetBlock();
	}
}

void AMinecraftCharacter::BreakBlock()
{
	if (bIsBreaking && CurrentBlock != nullptr && !CurrentBlock->IsPendingKill())
	{
		CurrentBlock->Break();
	}
}

void AMinecraftCharacter::UpdateWieldedItem()
{
	/* If the current inventory slot is not empty then set the mesh to that item, otherwise make it null */
	Inventory[CurrentInventorySlot] != NULL ? FP_WieldedItem->SetSkeletalMesh(Inventory[CurrentInventorySlot]->WieldableMesh->SkeletalMesh) : FP_WieldedItem->SetSkeletalMesh(NULL);
}

AWieldable * AMinecraftCharacter::GetCurrentWieldedItem()
{
	return Inventory[CurrentInventorySlot] != NULL ? Inventory[CurrentInventorySlot] : nullptr;
}

void AMinecraftCharacter::Throw()
{
	// Get Currently wielded item
	AWieldable* ItemToThrow = GetCurrentWieldedItem();

	FHitResult LineTraceHit;
	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
	FVector EndTrace = FirstPersonCameraComponent->GetForwardVector() * Reach + StartTrace;
	FCollisionQueryParams FCQP;
	FCQP.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(LineTraceHit, StartTrace, EndTrace, ECollisionChannel::ECC_WorldDynamic, FCQP);

	FVector DropLocation = EndTrace;
	if (LineTraceHit.GetActor())
	{
		DropLocation = LineTraceHit.ImpactPoint + 20.f;
	}

	if (ItemToThrow)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			ItemToThrow->SetActorLocationAndRotation(DropLocation, FRotator::ZeroRotator);
			ItemToThrow->Hide(false);

		}
	}
	UpdateWieldedItem();
}

void AMinecraftCharacter::MoveUpInventorySlot()
{
	CurrentInventorySlot = FMath::Abs((CurrentInventorySlot + 1) % NUM_OF_INVENTORY_SLOTS);
	UpdateWieldedItem();
}

void AMinecraftCharacter::MoveDownInventorySlot()
{
	if (CurrentInventorySlot == 0)
	{
		CurrentInventorySlot = 9;
		return;
	}
	CurrentInventorySlot = FMath::Abs((CurrentInventorySlot - 1) % NUM_OF_INVENTORY_SLOTS);
	UpdateWieldedItem();
}
