// Fill out your copyright notice in the Description page of Project Settings.

#include "Block.h"
#include "Chunk.h"
// #include "PickupBox.h"
#include "Wieldable.h"

// Sets default values
ABlock::ABlock() {
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GrassBlockSkeletalMeshAsset(TEXT("SkeletalMesh'/Game/Assets/Meshes/Blocks/SK_GrassBlock.SK_GrassBlock'"));

	AssetsManager = NewObject<AAssetsManager>(this, TEXT("AssetsManager"));
	SkeletalMeshAsset = GrassBlockSkeletalMeshAsset.Object;

	PrimaryActorTick.bCanEverTick = false;
	Resistance = 20.f;
	BreakingStage = 0.f;
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
		FRotator Rotation(0.0f, 0.0f, 0.0f);

		Cast<AChunk>(Chunk)->OnBlockDestroyed();
		Hide();

		AWieldable* SpawnedBox = GetWorld()->SpawnActor<AWieldable>(Location,  Rotation);

		SpawnedBox->WieldableMesh->SetSkeletalMesh(SkeletalMeshAsset);
		SpawnedBox->WieldableMesh->SetMaterial(0, SM_Block->GetMaterial(0));
		SpawnedBox->WieldableMesh->SetWorldScale3D(FVector(0.3, 0.3, 0.3));
		SpawnedBox->PickupThumbnail = AssetsManager->GetThumbnailByMaterial(SM_Block->GetMaterial(0)->GetMaterial());

		SpawnedBox->ToolType = ETool::Block;
		SpawnedBox->MaterialType = AssetsManager->GetTypeByMaterial(SM_Block->GetMaterial(0)->GetMaterial());
	}
}

void ABlock::Place(UMaterialInterface* Material, FVector RelativeLocation) {
	BreakingStage = 0.f;
	
	SM_Block->SetMaterial(0, Material);
	SM_Block->SetRelativeLocation(RelativeLocation);

	SM_Block->SetVisibility(true);
	SM_Block->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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

void ABlock::SetLocation(FVector NewLocation) {
	Location = NewLocation;
}
