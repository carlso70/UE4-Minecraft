// Fill out your copyright notice in the Description page of Project Settings.

#include "Minecraft.h"
#include "MinecraftCharacter.h"
#include "Wieldable.h"


// Sets default values
AWieldable::AWieldable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WieldableMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WieldableMesh"));
	
	// Can be any shape, as long as it inherits from the UShapeComponent class
	PickupTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupTrigger"));

	PickupTrigger->bGenerateOverlapEvents = true;
	PickupTrigger->OnComponentBeginOverlap.AddDynamic(this, &AWieldable::OnRadiusEnter);
	PickupTrigger->AttachToComponent(WieldableMesh, FAttachmentTransformRules::KeepRelativeTransform);

	MaterialType = EMaterial::None;
	ToolType = ETool::Unarmed;

	bIsActive = true;
}

// Called when the game starts or when spawned
void AWieldable::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWieldable::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	// Moves Pickup up and down every frame
	FRotator Rotation = WieldableMesh->GetComponentRotation();
	Rotation.Yaw += 1;
	WieldableMesh->SetRelativeRotation(Rotation);
}

void AWieldable::OnRadiusEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (bIsActive)
	{
		AMinecraftCharacter* Character = Cast<AMinecraftCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		Character->AddItemToInventory(this);

		Hide(true);
	}
}

void AWieldable::Hide(bool bVisibility)
{
	WieldableMesh->SetVisibility(bVisibility);
	bIsActive = !bVisibility;
}

void AWieldable::OnUsed()
{
	Destroy();
}