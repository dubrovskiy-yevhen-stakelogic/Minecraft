// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"

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

	for (int LoopX = 0; LoopX < NUM_X; LoopX++) {
		for (int LoopY = 0; LoopY < NUM_Y; LoopY++) {
			FTransform InstanceTransform;

			InstanceTransform.SetLocation(FVector(LoopX * VOXEL_SIZE, LoopY * VOXEL_SIZE, FMath::RandRange(-10, 10)));
			InstanceTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

			ISMComp->AddInstance(InstanceTransform);
		}
	}

	AddInstanceComponent(ISMComp);
}
