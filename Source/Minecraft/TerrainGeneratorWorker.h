// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Minecraft.h"

struct TerrainWorkerData {
    TArray<float> InitZPositions;
    int DEPTH;
    int NUM;
    int VOXEL_SIZE;
    FVector Location;
    TMap<FString, BlockInfo> *ChunkBlocks;
};
class MINECRAFT_API FTerrainGeneratorWorker : public FRunnable {
public:
	FTerrainGeneratorWorker();
	virtual ~FTerrainGeneratorWorker() override;

	// Overriden from FRunnable
	virtual  bool Init() override; // Do your setup here, allocate memory, ect.
	virtual  uint32 Run() override; // Main data processing happens here
	virtual  void Stop() override; // Clean up any memory you allocated here

	void SetWorkerData(TArray<float>, int, int,  int, FVector, TMap<FString, BlockInfo>*);	
private:
	// Thread handle. Control the thread using this, with operators like Kill and Suspend
	FRunnableThread* Thread;

	TArray<TerrainWorkerData> WorkerData;

	// Used to know when the thread should exit, changed in Stop(), read in Run()
	bool bRunThread;

	static void FillBlocksMap(const TArray<float>, const int, const int,  const int, const FVector, TMap<FString, BlockInfo>*);
};
