// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minecraft.h"
#include "TerrainGeneratorWorker.h"

/**
 * 
 */
class MINECRAFT_API TreeGenerator {
public:
	TreeGenerator();

	static void GenerateChunk(
		FVector Location,
		int NUM,
		int VOXEL_SIZE,
		int DEPTH,
		TArray<UInstancedStaticMeshComponent*> Instances,
		TMap<FString, bool> *ReservedTreePosMap,
		FTerrainGeneratorWorker* FWorker,
		TMap<FString, bool>* VisibleBlocks,
		TMap<FString, BlockInfo> *ChunkBlocks
	);

	static void GenerateTree(FVector, TMap<FString, bool>*, int, UInstancedStaticMeshComponent*, UInstancedStaticMeshComponent*);
};
