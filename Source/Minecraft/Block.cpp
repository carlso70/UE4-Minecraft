// Fill out your copyright notice in the Description page of Project Settings.

#include "Minecraft.h"
#include "Block.h"


// Sets default values
ABlock::ABlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	SM_Block = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));

	Resistance = 20.f;
	BreakingStage = 0.f;
	MinimumMaterial = 0;
}

// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlock::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ABlock::Break()
{
	BreakingStage++;
	
	float CrackingValue = 1.f - (BreakingStage / 5.f);

	UMaterialInstanceDynamic* MatInstance = SM_Block->CreateDynamicMaterialInstance(0);

	if (MatInstance)
	{
		MatInstance->SetScalarParameterValue(FName("CrackingValue"), CrackingValue);
	}

	if (BreakingStage == 5.f)
	{
		// Set to true right now, because we are assuming we always have the right pickaxe
		OnBroken(true);
	}
}

void ABlock::OnBroken(bool HasRequiredPickaxe)
{
	Destroy();
}

void ABlock::ResetBlock()
{
	BreakingStage = 0.f;

	UMaterialInstanceDynamic* MatInstance = SM_Block->CreateDynamicMaterialInstance(0);

	if (MatInstance != nullptr)
	{
		MatInstance->SetScalarParameterValue(FName("CrackingValue"), 1.f);
	}
}