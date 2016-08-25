// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Block.generated.h"

UCLASS()
class MINECRAFT_API ABlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlock();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// will set later to ETools::Diamond or something like that
	uint8 MinimumMaterial = 3;

	/* Static mesh for the block */
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* SM_Block;
		
	UPROPERTY(EditDefaultsOnly, Category = "Block Health")
	float Resistance;

	UPROPERTY(BlueprintReadWrite)
	float BreakingStage;

	/* Called everytime we want to break the block down further, break it down a stage */
	void Break();

	void ResetBlock();

	/* Called once the block has it the final breaking stage 
	@params HasRequiredPickaxe, some blocks require a certain level of pickaxe
	*/
	void OnBroken(bool HasRequiredPickaxe);
};
