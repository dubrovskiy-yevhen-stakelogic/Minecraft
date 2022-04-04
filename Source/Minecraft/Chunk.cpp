// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "SimplexNoise/SimplexNoiseBPLibrary.h"

// Sets default values
AChunk::AChunk() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SetActorEnableCollision(true);

	RootComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	BreakingBlock = NewObject<ABlock>(this, TEXT("Breaking block"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> UnitBlockAsset(TEXT("StaticMesh'/Game/Assets/Meshes/Blocks/SM_UnitBlock.SM_UnitBlock'"));

	AAssetsManager* AssetsManager = NewObject<AAssetsManager>(this, TEXT("AssetsManager"));

	EMaterial MaterialTypes[4] = {  EMaterial::Snow,  EMaterial::Grass,  EMaterial::Iron,  EMaterial::Cobble };
	int AssetsLength = sizeof MaterialTypes / sizeof MaterialTypes[0];

	UStaticMeshComponent* BreakingBlockMesh = NewObject<UStaticMeshComponent>(this, TEXT("Breaking mesh block"));

	BreakingBlock->SetStaticMesh(BreakingBlockMesh);
	BreakingBlock->SetChunk(this);
	BreakingBlockMesh->SetStaticMesh(UnitBlockAsset.Object);
	BreakingBlock->Hide();

	for (int i = 0; i < AssetsLength; i++) {
		Instances.Add(NewObject<UInstancedStaticMeshComponent>(this, AssetsManager->ConvertMaterialTypeToString(MaterialTypes[i])));
		Instances[i]->SetStaticMesh(UnitBlockAsset.Object);
		Instances[i]->SetMaterial(0, AssetsManager->GetMaterialByType(MaterialTypes[i]));
		Instances[i]->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

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
					Instances[0]->AddInstance(InstanceTransform);
				} else if (z >= -3) {
					Instances[1]->AddInstance(InstanceTransform);
				} else if (z == -4) {
					Instances[2]->AddInstance(InstanceTransform);
				} else {
					Instances[3]->AddInstance(InstanceTransform);
				}
			}
		}
	}
}

int AChunk::FindBlockInInstance(UInstancedStaticMeshComponent * InstanceComponent, FVector_NetQuantize HitLocation) {
	FVector Location = GetActorLocation();

	for (int i = InstanceComponent->GetInstanceCount() - 1; i >= 0; i--) {
		//Save the transform of current Instance.
		FTransform Out;
		InstanceComponent->GetInstanceTransform(i, Out);

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

	for (int i = 0; i < Instances.Num(); i++) {
		ind = FindBlockInInstance(Instances[i], HitLocation);
		if (ind != -1) {
			FTransform Out;
			Instances[i]->GetInstanceTransform(ind, Out);
			Instances[i]->RemoveInstance(ind);
			BrakeLocation = Out.GetLocation();
			BrakeInstanceInd = i;

			BreakingBlock->SetLocation(BrakeLocation + Location);

			BreakingBlock->Place(Instances[i]->GetMaterial(0), BrakeLocation);
			return;
		}
	}
}

void AChunk::OnBlockDestroyed() {
	BrakeInstanceInd = -1;
}

void AChunk::ResetBlock() {
	if (BrakeInstanceInd == -1) {
		return;
	}

	BreakingBlock->Hide();

	FTransform InstanceTransform;
	InstanceTransform.SetLocation(BrakeLocation);
	Instances[BrakeInstanceInd]->AddInstance(InstanceTransform);
	BrakeInstanceInd = -1;
}

void AChunk::Break() {
	if (BrakeInstanceInd == -1) {
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

