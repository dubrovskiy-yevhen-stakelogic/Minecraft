// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"
#include "Chunk.h"

// Sets default values
ABlock::ABlock()
{
	// SM_Block = NewObject<UStaticMeshComponent>(this, TEXT("Temporary block!")); // CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));

	Resistance = 20.f;
	BreakingStage = 0.f;
	MinimumMaterial = 0;
}

// Called when the game starts or when spawned
void ABlock::BeginPlay() {
	Super::BeginPlay();
}

UStaticMeshComponent* ABlock::GetStaticMesh() {
	return SM_Block;
}

void ABlock::SetChunk(class AActor* ParentChunk) {
	Chunk = ParentChunk;
}

void ABlock::SetStaticMesh(UStaticMeshComponent* block) {
	SM_Block = block;
}

void ABlock::Break() {
	++BreakingStage;

	float CrackingValue = 1.0f - (BreakingStage / 5.f);

	UMaterialInstanceDynamic* MatInstance = SM_Block->CreateDynamicMaterialInstance(0);

	if (MatInstance != nullptr) {
		MatInstance->SetScalarParameterValue(FName("CrackingValue"), CrackingValue);
	}

	if (BreakingStage == 5.f) {
		Cast<AChunk>(Chunk)->OnBlockDestroyed();
		Hide();
	}
}

void ABlock::Place(UStaticMesh* StaticMesh, FVector Location) {
	BreakingStage = 0.f;
	
	SM_Block->SetStaticMesh(StaticMesh);
	SM_Block->SetRelativeLocation(Location);

	SM_Block->SetVisibility(true);
	SM_Block->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABlock::Attach(class AActor *owner) {
	SM_Block->AttachToComponent(owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

void ABlock::Hide() {
	BreakingStage = 0.f;

	UMaterialInstanceDynamic* MatInstance = SM_Block->CreateDynamicMaterialInstance(0);

	if (MatInstance != nullptr) {
		MatInstance->SetScalarParameterValue(FName("CrackingValue"), 1.f);
	}
	
	SM_Block->SetVisibility(false);
	SM_Block->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABlock::ResetBlock() {
	Cast<AChunk>(Chunk)->ResetBlock();
	Hide();
}
