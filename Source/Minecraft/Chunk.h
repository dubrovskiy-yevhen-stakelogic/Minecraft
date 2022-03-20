// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Block.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"


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
	static const int CHUNK_SIZE_XY = 1000;
	UInstancedStaticMeshComponent *ISMComp;
private:
	int NUM_X = 10;
	int NUM_Y = 10;
	float VOXEL_SIZE = 100;	
};
