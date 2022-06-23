// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "TreeGenerator.h"
#include "Torch.h"

AAssetsManager* AChunk::AssetsManager;
TMap<FString, BlockInfo> * AChunk::Blocks;
FTerrainGeneratorWorker* AChunk::FWorker;
TMap<FString, AChunk*> * AChunk::SpawnedChunksMap;
TSubclassOf<class AActor> AChunk::BPTorchClass;

// Sets default values
AChunk::AChunk() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ChunkBlocks = new TMap<FString, BlockInfo>();
	VisibleBlocks = new TMap<FString, bool>();
	ReservedTreePosMap = new TMap<FString, bool>();

	SetActorEnableCollision(true);

	RootComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	BreakingBlock = CreateDefaultSubobject<ABlock>("Breaking block");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> UnitBlockAsset(TEXT("StaticMesh'/Game/Assets/Meshes/Blocks/SM_UnitBlock.SM_UnitBlock'"));	

	constexpr  EMaterial MaterialTypes[6] = {  EMaterial::Snow,  EMaterial::Grass,  EMaterial::Iron,  EMaterial::Cobble, EMaterial::WoodOak, EMaterial::Leaves };
	constexpr  int AssetsLength = sizeof MaterialTypes / sizeof MaterialTypes[0];

	UStaticMeshComponent* BreakingBlockMesh = CreateDefaultSubobject<UStaticMeshComponent>("Breaking mesh block");

	BreakingBlock->SetStaticMesh(BreakingBlockMesh);
	BreakingBlock->SetChunk(this);
	BreakingBlockMesh->SetStaticMesh(UnitBlockAsset.Object);
	BreakingBlock->Hide();

	for (int i = 0; i < AssetsLength; i++) {
		Instances.Add(CreateDefaultSubobject<UInstancedStaticMeshComponent>(AssetsManager->ConvertMaterialTypeToString(MaterialTypes[i])));
		Instances[i]->SetStaticMesh(UnitBlockAsset.Object);
		Instances[i]->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	
	BreakingBlock->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BreakingBlockMesh->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}


// Called when the game starts or when spawned
void AChunk::BeginPlay() {
	Super::BeginPlay();

	constexpr  EMaterial MaterialTypes[6] = {  EMaterial::Snow,  EMaterial::Grass,  EMaterial::Iron,  EMaterial::Cobble, EMaterial::WoodOak, EMaterial::Leaves };
	constexpr  int AssetsLength = sizeof MaterialTypes / sizeof MaterialTypes[0];

	for (int i = 0; i < AssetsLength; i++) {
		Instances[i]->SetMaterial(0, AssetsManager->GetMaterialByType(MaterialTypes[i]));
	}

	// MUCH OPTIMIZED
	const FVector Location = GetActorLocation();

	TreeGenerator::GenerateChunk(
		Location,
		NUM,
		VOXEL_SIZE,
		DEPTH,
		Instances,
		ReservedTreePosMap,
		FWorker,
		VisibleBlocks,
		ChunkBlocks
	);
}

int AChunk::FindBlockInInstance(const UInstancedStaticMeshComponent * InstanceComponent, FVector_NetQuantize HitLocation) const {
	const FVector Location = GetActorLocation();

	for (int i = InstanceComponent->GetInstanceCount() - 1; i >= 0; i--) {
		//Save the transform of current Instance.
		FTransform Out;
		InstanceComponent->GetInstanceTransform(i, Out);

		const FVector MeshLocation = Out.GetLocation() + Location + FVector(VOXEL_SIZE / -2, VOXEL_SIZE / -2, VOXEL_SIZE / -2);

		if (IsInBox(MeshLocation, HitLocation)) {
			return i;
		}
    }

	return -1;
}

void AChunk::PlaceBlock(FVector HitLocation, TEnumAsByte<EMaterial> MaterialType) {
	const FVector Location = GetActorLocation();

	for (int i = 0; i < Instances.Num(); i++) {
		const int Ind = FindBlockInInstance(Instances[i], HitLocation);
		if (Ind != -1) {
			FTransform Out;
			Instances[i]->GetInstanceTransform(Ind, Out);

			const FVector MeshLocation = Out.GetLocation() + Location + FVector(VOXEL_SIZE / -2, VOXEL_SIZE / -2, VOXEL_SIZE / -2);

			FVector PlaceLocation = Out.GetLocation();

			if (MeshLocation.X == HitLocation.X) {
				PlaceLocation += FVector(-VOXEL_SIZE, 0, 0);
			} else if (MeshLocation.X == HitLocation.X - VOXEL_SIZE) {
				PlaceLocation += FVector(VOXEL_SIZE, 0, 0);
			} else if (MeshLocation.Y == HitLocation.Y) {
				PlaceLocation += FVector(0, -VOXEL_SIZE, 0);
			} else if (MeshLocation.Y == HitLocation.Y - VOXEL_SIZE) {
				PlaceLocation += FVector(0, VOXEL_SIZE, 0);
			} else if (MeshLocation.Z == HitLocation.Z) {
				PlaceLocation += FVector(0, 0, -VOXEL_SIZE);
			} else if (MeshLocation.Z == HitLocation.Z - VOXEL_SIZE) {
				PlaceLocation += FVector(0, 0, VOXEL_SIZE);
			} else {
				return;
			}

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, AssetsManager->ConvertMaterialTypeToString((MaterialType)).ToString());

			if (MaterialType == EMaterial::TorchMat) {
				
				// const AActor* TorchClass = Cast<AActor>(AAssetsManager::BPTorch->GetClass());
				
				GetWorld()->SpawnActor<AActor>(BPTorchClass, PlaceLocation + Location + FVector(0, 0, -VOXEL_SIZE / 2), FRotator(0, 0, 0));

				/*
					SpawnedPickaxe->WieldableMesh->SetSkeletalMesh(AssetsManager->SkeletalPickaxe);
					SpawnedPickaxe->WieldableMesh->SetMaterial(0, AssetsManager->GetMaterialByType((EMaterial::DiamondPickaxe)));
					SpawnedPickaxe->PickupThumbnail = AssetsManager->GetThumbnailByMaterial(AssetsManager->GetMaterialByType((EMaterial::DiamondPickaxe)));
					SpawnedPickaxe->ToolType = ETool::Pickaxe;
					SpawnedPickaxe->MaterialType = EMaterial::DiamondPickaxe; 

				SpawnedTorch->WieldableMesh->SetSkeletalMesh(AssetsManager->SkeletalTorch);
				SpawnedTorch->WieldableMesh->SetMaterial(0, AssetsManager->GetMaterialByType((EMaterial::TorchMat)));
				SpawnedTorch->PickupThumbnail = AssetsManager->GetThumbnailByMaterial(AssetsManager->GetMaterialByType((EMaterial::TorchMat)));
				SpawnedTorch->ToolType = ETool::Torch;
				SpawnedTorch->MaterialType = EMaterial::TorchMat;
				*/

				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "PICKAXE SPAWNED");
				
				return;
			}

			for (int j = 0; j < Instances.Num(); j++) {
				if (AssetsManager->GetTypeByMaterial(Instances[j]->GetMaterial(0)->GetMaterial()) == MaterialType) {
					FTransform InstanceTransform;
					InstanceTransform.SetLocation(PlaceLocation);
					Instances[j]->AddInstance(InstanceTransform);
					return;
				}
			}

			return;
		}
	}
}

void AChunk::FindBlock(FVector_NetQuantize HitLocation) {
	const FVector Location = GetActorLocation();

	for (int i = 0; i < Instances.Num(); i++) {
		const int Ind = FindBlockInInstance(Instances[i], HitLocation);
		if (Ind != -1) {

			FTransform Out;
			Instances[i]->GetInstanceTransform(Ind, Out);
			Instances[i]->RemoveInstance(Ind);
			BrakeLocation = Out.GetLocation();
			BrakeInstanceInd = i;

			BreakingBlock->SetLocation(BrakeLocation + Location);

			BreakingBlock->Place(Instances[i]->GetMaterial(0), BrakeLocation);
			return;
		}
	}
}

void AChunk::ShowBlock(FVector BlockPosition) {
	if (!ChunkBlocks->Contains(BlockPosition.ToString())) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Block not found!");
		return;	
	}

	if (VisibleBlocks->Contains(BlockPosition.ToString())) {
		return;
	}

	const FVector Location = GetActorLocation();

	FTransform InstanceTransform;
	InstanceTransform.SetLocation(BlockPosition - Location);
	Instances[3]->AddInstance(InstanceTransform);

	VisibleBlocks->Add(BlockPosition.ToString(), true);
}

void AChunk::OnBlockDestroyed() {
	BrakeInstanceInd = -1;

	const FVector Location = GetActorLocation();
	const FVector MeshLocation = BrakeLocation + Location;

	if (!ChunkBlocks->Contains(MeshLocation.ToString())) {
		return;	
	}

	ChunkBlocks->Remove(MeshLocation.ToString());
	VisibleBlocks->Remove(MeshLocation.ToString());

	const FVector NeighborsPos[6] = { 
		MeshLocation + FVector(VOXEL_SIZE, 0, 0),
		MeshLocation + FVector(-VOXEL_SIZE, 0, 0),
		MeshLocation + FVector(0, VOXEL_SIZE, 0),
		MeshLocation + FVector(0, -VOXEL_SIZE, 0),
		MeshLocation + FVector(0, 0, VOXEL_SIZE),
		MeshLocation + FVector(0, 0, -VOXEL_SIZE),
	};

	const FString NeighborsChunksKeys[4] = {
		(Location + FVector(-AChunk::CHUNK_SIZE_XY, 0, 0)).ToString(),
		(Location + FVector(AChunk::CHUNK_SIZE_XY, 0, 0)).ToString(),
		(Location + FVector(0, -AChunk::CHUNK_SIZE_XY, 0)).ToString(),
		(Location + FVector(0, AChunk::CHUNK_SIZE_XY, 0)).ToString(),
	};

	for (int i = 0; i < 6; i++) {
		if (NeighborsPos[i].X < Location.X) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "BOTTOM OUT OF BOUND!!!");

			AChunk* NeighborChunk = SpawnedChunksMap->operator[](NeighborsChunksKeys[0]);
			
			if (NeighborChunk != nullptr) {
				NeighborChunk->ShowBlock(NeighborsPos[i]);
			}
			
			continue;
		} else if (NeighborsPos[i].X == Location.X + CHUNK_SIZE_XY) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "TOP OUT OF BOUND!!!");
			AChunk* NeighborChunk = SpawnedChunksMap->operator[](NeighborsChunksKeys[1]);
			
			if (NeighborChunk != nullptr) {
				NeighborChunk->ShowBlock(NeighborsPos[i]);
			}
			
			continue;
		} else if (NeighborsPos[i].Y < Location.Y) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "LEFT OUT OF BOUND!!!");
			AChunk* NeighborChunk = SpawnedChunksMap->operator[](NeighborsChunksKeys[2]);
			
			if (NeighborChunk != nullptr) {
				NeighborChunk->ShowBlock(NeighborsPos[i]);
			}
			continue;
		} else if (NeighborsPos[i].Y == Location.Y + CHUNK_SIZE_XY) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "RIGHT OUT OF BOUND!!!");
			AChunk* NeighborChunk = SpawnedChunksMap->operator[](NeighborsChunksKeys[3]);
			
			if (NeighborChunk != nullptr) {
				NeighborChunk->ShowBlock(NeighborsPos[i]);
			}
			continue;
		}

		if (ChunkBlocks->Contains(NeighborsPos[i].ToString()) && !VisibleBlocks->Contains(NeighborsPos[i].ToString())) {
			FTransform InstanceTransform;
			InstanceTransform.SetLocation(NeighborsPos[i] - Location);
			Instances[3]->AddInstance(InstanceTransform);

			VisibleBlocks->Add(NeighborsPos[i].ToString(), true);
		}
	}
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

void AChunk::Break() const {
	if (BrakeInstanceInd == -1) {
		return;
	}

	BreakingBlock->Break();
}

void AChunk::SetVisibility(bool IsVisible) {
	SetActorHiddenInGame(!IsVisible);
	SetActorEnableCollision(IsVisible);
}

bool AChunk::IsInBox(const FVector BoxLocation, const FVector_NetQuantize HitLocation) const {
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

