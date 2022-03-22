// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "SimplexNoise/SimplexNoiseBPLibrary.h"

// Sets default values
AChunk::AChunk() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetActorEnableCollision(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BaseMeshAsset(TEXT("StaticMesh'/Game/Assets/Meshes/Blocks/SM_Grassblock.SM_GrassBlock'"));
	ISMComp = NewObject<UInstancedStaticMeshComponent>(this, TEXT("Chunk Instance Manager"));
	ISMComp->SetStaticMesh(BaseMeshAsset.Object);
	RootComponent = ISMComp;
}

// Called when the game starts or when spawned
void AChunk::BeginPlay() {
	Super::BeginPlay();

	FVector Location = GetActorLocation();

	// float ActorX = Location.X / VOXEL_SIZE;
	// float ActorY = Location.Y / VOXEL_SIZE;

	for (int LoopX = 0; LoopX < NUM; LoopX++) {
		for (int LoopY = 0; LoopY < NUM; LoopY++) {
			// for (int LoopZ = 0; LoopZ < DEPTH; LoopZ++) {
				FTransform InstanceTransform;

				// float NoiseDensity = 0.0005;
				float NoiseDensity = 0.0002;
				int NoiseScale = 6;

				float X = LoopX * VOXEL_SIZE;
				float Y = LoopY * VOXEL_SIZE;

				float Z = (FMath::Floor(USimplexNoiseBPLibrary::SimplexNoise2D(X + Location.X, Y + Location.Y, NoiseDensity) * NoiseScale) ) * VOXEL_SIZE;
				// Z -= VOXEL_SIZE * LoopZ;

				InstanceTransform.SetLocation(FVector(X, Y, Z));
				InstanceTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

				ISMComp->AddInstance(InstanceTransform);
			// }
		}
	}

	AddInstanceComponent(ISMComp);
}

void AChunk::SetVisibility(bool IsVisible) {
	SetActorHiddenInGame(!IsVisible);
	SetActorEnableCollision(IsVisible);
}

