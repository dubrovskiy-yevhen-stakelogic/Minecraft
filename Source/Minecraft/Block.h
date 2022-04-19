// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetsManager.h"
#include "GameFramework/Actor.h"
#include "Block.generated.h"

UCLASS()
class MINECRAFT_API ABlock : public AActor {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlock();

private:
	FVector Location;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/* mesh of breaking block */
	UPROPERTY()
	UStaticMeshComponent* SM_Block;

	/* Chunk where we will put block */
	UPROPERTY()
	AActor* Chunk;

	void SetLocation(FVector);	

	float Resistance;
	float BreakingStage;

	void SetStaticMesh(UStaticMeshComponent*);
	UStaticMeshComponent* GetStaticMesh() const;

	void Break();
	void ResetBlock();

	void Hide();
	void Place(UMaterialInterface* Material, FVector Location);

	void SetChunk(AActor*);

	UPROPERTY()
	AAssetsManager* AssetsManager;
};
