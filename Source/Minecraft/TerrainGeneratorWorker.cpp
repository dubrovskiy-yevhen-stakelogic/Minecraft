// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainGeneratorWorker.h"

#pragma region Main Thread Code

FTerrainGeneratorWorker::FTerrainGeneratorWorker() {
    Thread = FRunnableThread::Create(this, TEXT("TerrainGeneratorWorker"));
}

FTerrainGeneratorWorker::~FTerrainGeneratorWorker() {
	if (Thread) {
		// Kill() is a blocking call, it waits for the thread to finish.
		// Hopefully that doesn't take too long
		Thread->Kill();
		delete Thread;
	}
}

void FTerrainGeneratorWorker::SetWorkerData(
    TArray<float> InitZPositions,
    int DEPTH,
    int NUM,
    int VOXEL_SIZE,
    FVector Location,
    TMap<FString, BlockInfo> *ChunkBlocks
) {
    TerrainWorkerData Data;

    Data.InitZPositions = InitZPositions;
    Data.DEPTH = DEPTH;
    Data.NUM = NUM;
    Data.VOXEL_SIZE = VOXEL_SIZE;
    Data.Location = Location;
    Data.ChunkBlocks = ChunkBlocks;

    WorkerData.Add(Data);
}

#pragma endregion

void FTerrainGeneratorWorker::FillBlocksMap(
    const TArray<float> InitZPositions,
    const int DEPTH,
    const int NUM,
    const int VOXEL_SIZE,
    const FVector Location,
    TMap<FString, BlockInfo> *ChunkBlocks
) {

    int Counter = 0;

    for (int LoopX = 0; LoopX < NUM; LoopX++) {
		for (int LoopY = 0; LoopY < NUM; LoopY++) {
            const float X = LoopX * VOXEL_SIZE;
			const float Y = LoopY * VOXEL_SIZE;
            const float InitZ = InitZPositions[LoopX * NUM + LoopY];

            for (int currentDepth = 1; currentDepth < DEPTH; currentDepth++) {
                const float Z = InitZ - currentDepth * VOXEL_SIZE;
                // int z = FMath::RoundToInt(Z / VOXEL_SIZE);

                BlockInfo CurrentBlockInfo;
                CurrentBlockInfo.Neighbors = 0;
                CurrentBlockInfo.Position = Location + FVector(X, Y, Z);
                CurrentBlockInfo.IsBottom = currentDepth == DEPTH - 1;
                CurrentBlockInfo.MaterialType = EMaterial::Cobble;

                if (!ChunkBlocks->Contains(CurrentBlockInfo.Position.ToString())) {
                    ++Counter;
                } else {
	                 // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, CurrentBlockInfo.Position.ToString());
                }
                
                ChunkBlocks->Add(CurrentBlockInfo.Position.ToString(), CurrentBlockInfo);
                // TODO: do not forgot about trees
                // Blocks->Add(CurrentBlockInfo.Position.ToString(), CurrentBlockInfo);
            }
        }
    }

    if (ChunkBlocks->Num() < 65000) {


        // FString toString = FString::Printf(TEXT("ChunkBlocks=%d"), ChunkBlocks->Num());
	    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, toString);

        // toString = FString::Printf(TEXT("Counter=%d"), Counter);
	    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Location.ToString());
    }

}

bool FTerrainGeneratorWorker::Init() {
	UE_LOG(LogTemp, Warning, TEXT("My custom thread has been initialized"))

	// Return false if you want to abort the thread
	return true;
}


uint32 FTerrainGeneratorWorker::Run() {
	while (bRunThread) {
        while (WorkerData.Num() > 0) {
            FillBlocksMap(
                WorkerData[0].InitZPositions,
                WorkerData[0].DEPTH,
                WorkerData[0].NUM,
                WorkerData[0].VOXEL_SIZE,
                WorkerData[0].Location,
                WorkerData[0].ChunkBlocks
            );

            WorkerData.RemoveAt(0);
        }

	    FPlatformProcess::Sleep(0.01);
	}

	return 0;
}


// This function is NOT run on the new thread!
void FTerrainGeneratorWorker::Stop() {
	// Clean up memory usage here, and make sure the Run() function stops soon
	// The main thread will be stopped until this finishes!

	// For this example, we just need to terminate the while loop
	// It will finish in <= 1 sec, due to the Sleep()
	bRunThread = false;
}
