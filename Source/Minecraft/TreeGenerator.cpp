// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeGenerator.h"
#include "SimplexNoise/SimplexNoiseBPLibrary.h"

TreeGenerator::TreeGenerator() { }

void TreeGenerator::GenerateTree(
	FVector Center, 
	TMap<FString, bool> *ReservedTreePosMap, 
	int VOXEL_SIZE, UInstancedStaticMeshComponent* TreeTrunkInstance,
	UInstancedStaticMeshComponent* LeavesInstance
) {
    	// generate tree
	if (ReservedTreePosMap->Contains(FVector(Center.X, Center.Y, 0).ToString())) {
		return;
	}

	const float StartX = Center.X - VOXEL_SIZE * 5;
	const float StartY = Center.Y - VOXEL_SIZE * 5;

	for (int i = 0; i < 10; i++) {
		const float X = StartX + i * VOXEL_SIZE;
		for (int j = 0; j < 10; j++) {
			const float Y = StartY + j * VOXEL_SIZE;
			ReservedTreePosMap->Add(FVector(X, Y, 0).ToString());
		}
	}

	float TreeX = Center.X;
	float TreeY = Center.Y;
	float TreeZ = Center.Z + VOXEL_SIZE;

	// first level 

	for (int i = 0; i < 5; i++) {
		FTransform TreeInstanceTransform;
		TreeInstanceTransform.SetLocation(FVector(TreeX, TreeY, TreeZ));
		TreeZ += VOXEL_SIZE;
		TreeTrunkInstance->AddInstance(TreeInstanceTransform);
	}

	TreeX -= 2 * VOXEL_SIZE;
	TreeY -= 2 * VOXEL_SIZE;

	// first level
	for (int g = 0; g < 5; g++) {
		const float LocalTreeX = TreeX + VOXEL_SIZE * g;

		for (int h = 0; h < 5; h++) {
			const float LocalTreeY = TreeY + VOXEL_SIZE * h;

			FTransform TreeInstanceTransform;
			TreeInstanceTransform.SetLocation(FVector(LocalTreeX, LocalTreeY, TreeZ));
			LeavesInstance->AddInstance(TreeInstanceTransform);
		}
	}

	TreeX += VOXEL_SIZE;
	TreeY += VOXEL_SIZE;
	TreeZ += VOXEL_SIZE;

	// second lvl
	for (int g = 0; g < 3; g++) {
		const float LocalTreeX = TreeX + VOXEL_SIZE * g;

		for (int h = 0; h < 3; h++) {
			const float LocalTreeY = TreeY + VOXEL_SIZE * h;

			FTransform TreeInstanceTransform;
			TreeInstanceTransform.SetLocation(FVector(LocalTreeX, LocalTreeY, TreeZ));
			LeavesInstance->AddInstance(TreeInstanceTransform);
		}
	}
}

void TreeGenerator::GenerateChunk(
	FVector Location,
	int NUM,
	int VOXEL_SIZE,
	int DEPTH,
	TArray<UInstancedStaticMeshComponent*> Instances,
	TMap<FString, bool> *ReservedTreePosMap,
	FTerrainGeneratorWorker* FWorker,
	TMap<FString, bool>* VisibleBlocks,
	TMap<FString, BlockInfo> *ChunkBlocks
) {
	TArray<float> InitZPositions;

	for (int LoopX = 0; LoopX < NUM; LoopX++) {
		for (int LoopY = 0; LoopY < NUM; LoopY++) {
			constexpr float NoiseDensity =  0.00025;
			constexpr int NoiseScale = 6;
			
			const float X = LoopX * VOXEL_SIZE;
			const float Y = LoopY * VOXEL_SIZE;

			float InitZ = (FMath::Floor(USimplexNoiseBPLibrary::SimplexNoise2D(X + Location.X, Y + Location.Y, NoiseDensity) * NoiseScale) ) * VOXEL_SIZE;

			InitZPositions.Add(InitZ);

			float Z = InitZ;
			// int z = FMath::RoundToInt(Z / VOXEL_SIZE);

			BlockInfo CurrentBlockInfo;
			CurrentBlockInfo.Neighbors = 0;
			CurrentBlockInfo.Position = Location + FVector(X, Y, Z);
			CurrentBlockInfo.IsBottom = false;

			// grass
			const bool IsShouldPlaceTree = FMath::RandRange(0, 100) == 0;

			if (IsShouldPlaceTree) {
				GenerateTree(FVector(X, Y, Z), ReservedTreePosMap, VOXEL_SIZE, Instances[4], Instances[5]);
			}

			CurrentBlockInfo.MaterialType = EMaterial::Grass;

			FTransform InstanceTransform;
			InstanceTransform.SetLocation(FVector(X, Y, Z));
			Instances[1]->AddInstance(InstanceTransform);
			
			ChunkBlocks->Add(CurrentBlockInfo.Position.ToString(), CurrentBlockInfo);
			VisibleBlocks->Add(CurrentBlockInfo.Position.ToString(), true);

			// TODO: do not forgot about trees
			// Blocks->Add(CurrentBlockInfo.Position.ToString(), CurrentBlockInfo);
		}
	}

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "PassedLocationToWorker=" + Location.ToString());

	FWorker->SetWorkerData(InitZPositions, DEPTH, NUM, VOXEL_SIZE, Location, ChunkBlocks);
}
