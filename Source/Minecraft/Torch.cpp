// Fill out your copyright notice in the Description page of Project Settings.


#include "Torch.h"

#include "AssetsManager.h"

// Sets default values
ATorch::ATorch() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkeletalMesh"));
}

// Called when the game starts or when spawned
void ATorch::BeginPlay() {
	Super::BeginPlay();

	StaticMesh->SetStaticMesh(AAssetsManager::StaticTorch);
	// StaticMesh->SetMaterial(0, AAssetsManager::GetMaterialByType((EMaterial::TorchMat)));
	
	// SkeletalMesh->SetSkeletalMesh(AAssetsManager::SkeletalTorch);
	// SkeletalMesh->SetMaterial(0, AAssetsManager::GetMaterialByType((EMaterial::TorchMat)));
	
	FRotator Rotation = StaticMesh->GetComponentRotation();
	Rotation.Roll += 90;
	StaticMesh->SetRelativeRotation(Rotation);

	const FVector RelativeLocation = StaticMesh->GetRelativeLocation();
	StaticMesh->SetRelativeLocation(RelativeLocation + FVector(0, 0, 16));
}

// Called every frame
void ATorch::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

