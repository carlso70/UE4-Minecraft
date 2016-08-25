// Fill out your copyright notice in the Description page of Project Settings.

#include "Minecraft.h"
#include "Wieldable.h"


// Sets default values
AWieldable::AWieldable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

}

