// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Wieldable.generated.h"

UCLASS()
class MINECRAFT_API AWieldable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWieldable();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	enum ETool : uint8
	{
		Unarmed,
		Pickaxe,
		Axe,
		Shovel,
		Sword
	};

	enum EMaterial : uint8
	{
		// Using 1, 2, 4, 6, 8, 12 to determine how fast blocks will break
		None = 1,
		Wooden = 2,
		Stone = 4,
		Iron = 6,
		Diamond = 8,
		Golden = 12
	};

	UPROPERTY(EditAnywhere)
	uint8 ToolType;

	UPROPERTY(EditAnywhere)
	uint8 MaterialType;

	// Tigger to toggle pickup
	UPROPERTY(EditAnywhere)
	UShapeComponent* PickupTrigger;
	
	// Mesh for the wieldable
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* WieldableMesh;

	/* Thumbnail that is displayed in the inventory */
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* PickupThumbnail;

	UFUNCTION()
	void OnRadiusEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	bool bIsActive;

	void Hide(bool bVisible);

	void OnUsed();
};
