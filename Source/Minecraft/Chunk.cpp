// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "SimplexNoise/SimplexNoiseBPLibrary.h"

// Sets default values
AChunk::AChunk() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetActorEnableCollision(true);

	RootComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> GrassBlockAsset(TEXT("StaticMesh'/Game/Assets/Meshes/Blocks/SM_Grassblock.SM_GrassBlock'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> IronBlockAsset(TEXT("StaticMesh'/Game/Assets/Meshes/Blocks/SM_IronBlock.SM_IronBlock'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CoobleBlockAsset(TEXT("StaticMesh'/Game/Assets/Meshes/Blocks/SM_CobbleBlock.SM_CobbleBlock'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SnowBlockAsset(TEXT("StaticMesh'/Game/Assets/Meshes/Blocks/SM_SnowBlock.SM_SnowBlock'"));

	InstancedGrass = NewObject<UInstancedStaticMeshComponent>(this, TEXT("Instanced Grass blocks"));
	InstancedCooble = NewObject<UInstancedStaticMeshComponent>(this, TEXT("Instanced Cobble blocks"));
	InstancedIron = NewObject<UInstancedStaticMeshComponent>(this, TEXT("Instanced Iron blocks"));
	InstancedSnow = NewObject<UInstancedStaticMeshComponent>(this, TEXT("Instanced Snow blocks"));

	InstancedCooble->SetStaticMesh(CoobleBlockAsset.Object);
	InstancedGrass->SetStaticMesh(GrassBlockAsset.Object);
	InstancedIron->SetStaticMesh(IronBlockAsset.Object);
	InstancedSnow->SetStaticMesh(SnowBlockAsset.Object);

	InstancedCooble->AttachTo(this->RootComponent);
	InstancedGrass->AttachTo(this->RootComponent);
	InstancedIron->AttachTo(this->RootComponent);
	InstancedSnow->AttachTo(this->RootComponent);
}

// Called when the game starts or when spawned
void AChunk::BeginPlay() {
	Super::BeginPlay();

	FVector Location = GetActorLocation();

	float NoiseDensity =  0.00025;// FMath::RandRange(1.0f, 3.0f) / 10000.0f;
	int NoiseScale = 6; // FMath::RandRange(2, 6);

	for (int LoopX = 0; LoopX < NUM; LoopX++) {
		for (int LoopY = 0; LoopY < NUM; LoopY++) {
			// for (int LoopZ = 0; LoopZ < DEPTH; LoopZ++) {
				FTransform InstanceTransform;

				float X = LoopX * VOXEL_SIZE;
				float Y = LoopY * VOXEL_SIZE;

				float Z = (FMath::Floor(USimplexNoiseBPLibrary::SimplexNoise2D(X + Location.X, Y + Location.Y, NoiseDensity) * NoiseScale) ) * VOXEL_SIZE;

				InstanceTransform.SetLocation(FVector(X, Y, Z));
				InstanceTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

				int z = FMath::RoundToInt(Z / VOXEL_SIZE);

				if (z >= 5) {
					InstancedSnow->AddInstance(InstanceTransform);
				} else if (z >= -3) {
					InstancedGrass->AddInstance(InstanceTransform);
				} else if (z == -4) {
					InstancedIron->AddInstance(InstanceTransform);
				} else {
					InstancedCooble->AddInstance(InstanceTransform);
				}


			// }
		}
	}
}

void AChunk::SetVisibility(bool IsVisible) {
	SetActorHiddenInGame(!IsVisible);
	SetActorEnableCollision(IsVisible);
}

