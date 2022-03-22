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
	static const int CHUNK_SIZE_XY = 2000;
	UInstancedStaticMeshComponent *ISMComp;

	void SetVisibility(bool);
private:
	int NUM = 20;
	int DEPTH = 5;
	float VOXEL_SIZE = 100;	
};
