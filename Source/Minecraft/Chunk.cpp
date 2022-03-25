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

	BreakingBlock = NewObject<UStaticMeshComponent>(this, TEXT("Temporary block!"));

	InstancedCooble->SetStaticMesh(CoobleBlockAsset.Object);
	InstancedGrass->SetStaticMesh(GrassBlockAsset.Object);
	InstancedIron->SetStaticMesh(IronBlockAsset.Object);
	InstancedSnow->SetStaticMesh(SnowBlockAsset.Object);

	InstancedCooble->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	InstancedGrass->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	InstancedIron->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	InstancedSnow->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BreakingBlock->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AChunk::BeginPlay() {
	Super::BeginPlay();

	FVector Location = GetActorLocation();

	float NoiseDensity =  0.00025;// FMath::RandRange(1.0f, 3.0f) / 10000.0f;
	int NoiseScale = 6; // FMath::RandRange(2, 6);

	int counter = 0;

	for (int LoopX = 0; LoopX < NUM; LoopX++) {
		for (int LoopY = 0; LoopY < NUM; LoopY++) {
			// for (int LoopZ = 0; LoopZ < DEPTH; LoopZ++) {
				FTransform InstanceTransform;

				float X = LoopX * VOXEL_SIZE;
				float Y = LoopY * VOXEL_SIZE;

				float Z = (FMath::Floor(USimplexNoiseBPLibrary::SimplexNoise2D(X + Location.X, Y + Location.Y, NoiseDensity) * NoiseScale) ) * VOXEL_SIZE;

				InstanceTransform.SetLocation(FVector(X, Y, Z));

				int z = FMath::RoundToInt(Z / VOXEL_SIZE);

				struct MyStruct s;

				s.Location = FVector(X + Location.X, Y + Location.Y, Z);

				if (z >= 5) {
					InstancedSnow->AddInstance(InstanceTransform);
				} else if (z >= -3) {
					InstancedGrass->AddInstance(InstanceTransform);
				} else if (z == -4) {
					InstancedIron->AddInstance(InstanceTransform);
				} else {
					InstancedCooble->AddInstance(InstanceTransform);
				}

				// InstancedSnow->UpdateInstanceTransform();

			// }
		}
	}
}

void AChunk::FindBlock(FVector_NetQuantize HitLocation) {	
	FVector Location = GetActorLocation();

	for (int i = InstancedSnow->GetInstanceCount() - 1; i >= 0; i--) {
		//Save the transform of current Instance.
		FTransform Out;
		InstancedSnow->GetInstanceTransform(i, Out);

		FVector MeshLocation = Out.GetLocation() + Location + FVector(VOXEL_SIZE / -2, VOXEL_SIZE / -2, VOXEL_SIZE / -2);

		if (IsInBox(MeshLocation, HitLocation)) {
			InstancedSnow->RemoveInstance(i);

			BrakeLocation = Out.GetLocation();
			BrakeInstance = InstancedSnow;

			PlaceTemporaryBlock();

			return;
		}
    }
}
void AChunk::ResetBlock() {
	if (BrakeInstance == nullptr) {
		return;
	}

	BreakingStage = 0;
	BreakingBlock->SetVisibility(false);

	FTransform InstanceTransform;
	InstanceTransform.SetLocation(BrakeLocation);

	BrakeInstance->AddInstance(InstanceTransform);

	BrakeInstance = NULL;
}

void AChunk::PlaceTemporaryBlock() {
	BreakingBlock->SetStaticMesh(BrakeInstance->GetStaticMesh());
	BreakingBlock->SetRelativeLocation(BrakeLocation);
	BreakingBlock->SetVisibility(true);
	BreakingBlock->SetActive(true);
	BreakingBlock->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AChunk::DestroyTMPBlock() {
	BreakingStage = 0;
	BrakeInstance = NULL;
	BreakingBlock->SetVisibility(false);
	BreakingBlock->SetActive(false);
	BreakingBlock->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AChunk::Break() {
	if (BrakeInstance == nullptr) {
		return;
	}

	++BreakingStage;

	float CrackingValue = 1.0f - (BreakingStage / 5.f);

	UMaterialInstanceDynamic* MatInstance = BreakingBlock->CreateDynamicMaterialInstance(0);

	if (MatInstance != nullptr) {
		MatInstance->SetScalarParameterValue(FName("CrackingValue"), CrackingValue);
	}

	if (BreakingStage == 5.f) {
		DestroyTMPBlock();
	}
}

void AChunk::SetVisibility(bool IsVisible) {
	SetActorHiddenInGame(!IsVisible);
	SetActorEnableCollision(IsVisible);
}

bool AChunk::IsInBox(FVector BoxLocation, FVector_NetQuantize HitLocation) {
	bool IsFound = true;

	if (HitLocation.X < BoxLocation.X || HitLocation.X > (BoxLocation.X + VOXEL_SIZE)) {
		IsFound = false;
	}

	if (HitLocation.Y < BoxLocation.Y || HitLocation.Y > (BoxLocation.Y + VOXEL_SIZE)) {
		IsFound = false;
	}

	if (HitLocation.Z < BoxLocation.Z || HitLocation.Z > (BoxLocation.Z + VOXEL_SIZE)) {
		IsFound = false;
	}

	return IsFound;
}

