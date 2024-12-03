// Fill out your copyright notice in the Description page of Project Settings.


#include "Sword_2H.h"

// Sets default values
ASword_2H::ASword_2H()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ASword_2H::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASword_2H::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

