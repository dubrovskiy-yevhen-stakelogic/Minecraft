// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Block.generated.h"

UCLASS()
class MINECRAFT_API ABlock : public AActor {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	uint8 MinimumMaterial;
	UStaticMeshComponent* SM_Block;
	AActor* Chunk;

	UPROPERTY(EditDefaultsOnly)
	float Resistance;


	UPROPERTY(BlueprintReadWrite)
	float BreakingStage;


	void SetStaticMesh(UStaticMeshComponent*);
	UStaticMeshComponent* GetStaticMesh();

	void Break();
	void ResetBlock();

	void Hide();
	void Place(UMaterialInterface* Material, FVector Location);

	void Attach(class AActor *owner);

	void SetChunk(AActor*);

	void OnBroken(bool HasRequiredPickaxe);
};
