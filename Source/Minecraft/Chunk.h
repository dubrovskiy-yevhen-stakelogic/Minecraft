// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Minecraft.h"
#include "AssetsManager.h"
#include "CoreMinimal.h"
#include "Block.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"

/*
0 - snow 
1 - grass
2 - iron
3 - cobble
*/

UCLASS()
class MINECRAFT_API AChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunk();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	static const int CHUNK_SIZE_XY = 1600;

	void FindBlock(FVector_NetQuantize);
	void SetVisibility(bool);

	void ResetBlock();
	void Break();
	void DestroyTMPBlock();

	void OnBlockDestroyed();

private:
	int NUM = 16;
	int DEPTH = 2;
	float VOXEL_SIZE = 100;

	FVector BrakeLocation;
	int BrakeInstanceInd;

	TArray<UInstancedStaticMeshComponent*> Instances;

	bool IsInBox(FVector, FVector_NetQuantize);

	int FindBlockInInstance(UInstancedStaticMeshComponent *, FVector_NetQuantize);

	ABlock* BreakingBlock;
};
