// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "SimplexNoise/SimplexNoiseBPLibrary.h"

// Sets default values
AChunk::AChunk() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetActorEnableCollision(true);

	RootComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	BreakingBlock = NewObject<ABlock>(this, TEXT("Breaking block")); 

	static ConstructorHelpers::FObjectFinder<UStaticMesh> GrassBlockAsset(TEXT("StaticMesh'/Game/Assets/Meshes/Blocks/SM_Grassblock.SM_GrassBlock'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> IronBlockAsset(TEXT("StaticMesh'/Game/Assets/Meshes/Blocks/SM_IronBlock.SM_IronBlock'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CoobleBlockAsset(TEXT("StaticMesh'/Game/Assets/Meshes/Blocks/SM_CobbleBlock.SM_CobbleBlock'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SnowBlockAsset(TEXT("StaticMesh'/Game/Assets/Meshes/Blocks/SM_SnowBlock.SM_SnowBlock'"));

	InstancedGrass = NewObject<UInstancedStaticMeshComponent>(this, TEXT("Instanced Grass blocks"));
	InstancedCooble = NewObject<UInstancedStaticMeshComponent>(this, TEXT("Instanced Cobble blocks"));
	InstancedIron = NewObject<UInstancedStaticMeshComponent>(this, TEXT("Instanced Iron blocks"));
	InstancedSnow = NewObject<UInstancedStaticMeshComponent>(this, TEXT("Instanced Snow blocks"));

	UStaticMeshComponent* BreakingBlockMesh = NewObject<UStaticMeshComponent>(this, TEXT("Temporary block!"));

	BreakingBlock->SetStaticMesh(BreakingBlockMesh);
	BreakingBlock->SetChunk(this);

	InstancedCooble->SetStaticMesh(CoobleBlockAsset.Object);
	InstancedGrass->SetStaticMesh(GrassBlockAsset.Object);
	InstancedIron->SetStaticMesh(IronBlockAsset.Object);
	InstancedSnow->SetStaticMesh(SnowBlockAsset.Object);

	InstancedCooble->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	InstancedGrass->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	InstancedIron->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	InstancedSnow->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	BreakingBlockMesh->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AChunk::BeginPlay() {
	Super::BeginPlay();

	FVector Location = GetActorLocation();

	float NoiseDensity =  0.00025;
	int NoiseScale = 6;

	for (int LoopX = 0; LoopX < NUM; LoopX++) {
		for (int LoopY = 0; LoopY < NUM; LoopY++) {
			FTransform InstanceTransform;

			float X = LoopX * VOXEL_SIZE;
			float Y = LoopY * VOXEL_SIZE;

			float InitZ = (FMath::Floor(USimplexNoiseBPLibrary::SimplexNoise2D(X + Location.X, Y + Location.Y, NoiseDensity) * NoiseScale) ) * VOXEL_SIZE;

			for (int currentDepth = 0; currentDepth < DEPTH; currentDepth++) { 
				float Z = InitZ - currentDepth * VOXEL_SIZE;

				InstanceTransform.SetLocation(FVector(X, Y, Z));

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
			}
		}
	}
}

int AChunk::FindBlockInInstance(UInstancedStaticMeshComponent * Instances, FVector_NetQuantize HitLocation) {
	FVector Location = GetActorLocation();

	for (int i = Instances->GetInstanceCount() - 1; i >= 0; i--) {
		//Save the transform of current Instance.
		FTransform Out;
		Instances->GetInstanceTransform(i, Out);

		FVector MeshLocation = Out.GetLocation() + Location + FVector(VOXEL_SIZE / -2, VOXEL_SIZE / -2, VOXEL_SIZE / -2);

		if (IsInBox(MeshLocation, HitLocation)) {
			return i;
		}
    }

	return -1;
}

void AChunk::FindBlock(FVector_NetQuantize HitLocation) {
	FVector Location = GetActorLocation();
	int ind = -1;

	UInstancedStaticMeshComponent *InstancesArray[4] = { InstancedSnow, InstancedGrass, InstancedIron, InstancedCooble };

	for (int i = 0; i < 4; i++) {
		ind = FindBlockInInstance(InstancesArray[i], HitLocation);
		if (ind != -1) {
			FTransform Out;
			InstancesArray[i]->GetInstanceTransform(ind, Out);
			InstancesArray[i]->RemoveInstance(ind);

			BrakeLocation = Out.GetLocation();
			BrakeInstance = InstancesArray[i];

			BreakingBlock->Place(InstancesArray[i]->GetStaticMesh(), BrakeLocation);
			return;
		}
	}
}

void AChunk::OnBlockDestroyed() {
	BrakeInstance = NULL;
}

void AChunk::ResetBlock() {
	if (BrakeInstance == nullptr) {
		return;
	}

	BreakingBlock->Hide();

	FTransform InstanceTransform;
	InstanceTransform.SetLocation(BrakeLocation);
	BrakeInstance->AddInstance(InstanceTransform);
	BrakeInstance = NULL;
}

void AChunk::Break() {
	if (BrakeInstance == nullptr) {
		return;
	}

	BreakingBlock->Break();
}

void AChunk::SetVisibility(bool IsVisible) {
	SetActorHiddenInGame(!IsVisible);
	SetActorEnableCollision(IsVisible);
}

bool AChunk::IsInBox(FVector BoxLocation, FVector_NetQuantize HitLocation) {
	if (HitLocation.X < BoxLocation.X || HitLocation.X > (BoxLocation.X + VOXEL_SIZE)) {
		return false;
	}

	if (HitLocation.Y < BoxLocation.Y || HitLocation.Y > (BoxLocation.Y + VOXEL_SIZE)) {
		return false;
	}

	if (HitLocation.Z < BoxLocation.Z || HitLocation.Z > (BoxLocation.Z + VOXEL_SIZE)) {
		return false;
	}

	return true;
}

