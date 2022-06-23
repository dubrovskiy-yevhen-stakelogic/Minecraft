// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Minecraft.h"
#include "AssetsManager.h"
#include "CoreMinimal.h"
#include "Block.h"
#include "TerrainGeneratorWorker.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"

UCLASS()
class MINECRAFT_API AChunk : public AActor {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunk();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	static AAssetsManager* AssetsManager;
	static TMap<FString, BlockInfo> *Blocks;
	static FTerrainGeneratorWorker* FWorker;
	static TMap<FString, AChunk*> *SpawnedChunksMap;

	static TSubclassOf<class AActor> BPTorchClass;
	
	static const int CHUNK_SIZE_XY = 1600;

	void FindBlock(FVector_NetQuantize);
	void SetVisibility(bool);

	void ResetBlock();
	void Break() const;

	void OnBlockDestroyed();
	void PlaceBlock(FVector HitLocation, TEnumAsByte<EMaterial> MaterialType);

	void ShowBlock(FVector BlockPosition);

	// void RenderChunk(TMap<FString, BlockInfo> Blocks, bool isIgnoreFront, bool isIgnoreBack, bool IsIgnoreLeft, bool IsIgnoreRight);

private:
	int NUM = 16;
	int DEPTH = 256;
	float VOXEL_SIZE = 100;

	FVector BrakeLocation;
	int BrakeInstanceInd;

	UPROPERTY()
	TArray<UInstancedStaticMeshComponent*> Instances;

	bool IsInBox(const FVector, const FVector_NetQuantize) const;

	int FindBlockInInstance(const UInstancedStaticMeshComponent *, FVector_NetQuantize) const;

	TMap<FString, bool>* VisibleBlocks;
	TMap<FString, BlockInfo> *ChunkBlocks;

	UPROPERTY()
	ABlock* BreakingBlock;

	// TODO: place in character
	TMap<FString, bool>* ReservedTreePosMap;
};
