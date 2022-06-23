// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetsManager.h"

USkeletalMesh* AAssetsManager::SkeletalBlock;
USkeletalMesh* AAssetsManager::SkeletalTorch;
USkeletalMesh* AAssetsManager::SkeletalPickaxe;
TArray<MaterialItem> AAssetsManager::Materials;
UStaticMesh* AAssetsManager::StaticTorch;
UBlueprint* AAssetsManager::BPTorch;

// Sets default values
AAssetsManager::AAssetsManager() {
	PrimaryActorTick.bCanEverTick = false;

	// StaticMesh'/Game/Assets/Meshes/Wieldables/SM_Torch.SM_Torch'

	static ConstructorHelpers::FObjectFinder<UBlueprint> TorchBPAsset(TEXT("Blueprint'/Game/Assets/Blueprints/BP_Torch.BP_Torch'"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> TorchStaticMeshAsset(TEXT("StaticMesh'/Game/Assets/Meshes/Wieldables/SM_Torch.SM_Torch'"));
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GrassBlockSkeletalMeshAsset(TEXT("SkeletalMesh'/Game/Assets/Meshes/Blocks/SK_GrassBlock.SK_GrassBlock'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TorchSkeletalMeshAsset(TEXT("SkeletalMesh'/Game/Assets/Meshes/Wieldables/SK_Torch.SK_Torch'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> PickaxeSkeletalMeshAsset(TEXT("SkeletalMesh'/Game/Assets/Meshes/Wieldables/SK_Pickaxe_Diamond.SK_Pickaxe_Diamond'"));

	static ConstructorHelpers::FObjectFinder<UMaterial> PickaxeMaterialAsset(TEXT("Material'/Game/Assets/Materials/Items/M_Pickaxe_Diamond.M_Pickaxe_Diamond'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> TorchMaterialAsset(TEXT("Material'/Game/Assets/Materials/Items/M_Torch.M_Torch'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> GrassBlockAsset(TEXT("Material'/Game/Assets/Materials/Nature/M_Grass.M_Grass'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> IronBlockAsset(TEXT("Material'/Game/Assets/Materials/Nature/M_Iron.M_Iron'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> SnowBlockAsset(TEXT("Material'/Game/Assets/Materials/Nature/M_Snow.M_Snow'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> CobbleBlockAsset(TEXT("Material'/Game/Assets/Materials/Nature/M_Cobble.M_Cobble'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> LeavesBlockAsset(TEXT("Material'/Game/Assets/Materials/Nature/M_Leaves.M_Leaves'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> WoodBlockAsset(TEXT("Material'/Game/Assets/Materials/Nature/M_Wood.M_Wood'"));
	
	static ConstructorHelpers::FObjectFinder<UTexture2D> GrassTextureAsset(TEXT("Texture2D'/Game/Assets/Textures/T_GrassThumbnail.T_GrassThumbnail'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IronTextureAsset(TEXT("Texture2D'/Game/Assets/Textures/T_IronThumbnail.T_IronThumbnail'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> SnowTextureAsset(TEXT("Texture2D'/Game/Assets/Textures/T_SnowThumbnail.T_SnowThumbnail'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> CobbleTextureAsset(TEXT("Texture2D'/Game/Assets/Textures/T_CobbleThumbnail.T_CobbleThumbnail'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> LeavesTextureAsset(TEXT("Texture2D'/Game/Textures/T_Ground_GrassThumbnail.T_Ground_GrassThumbnail'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> WoodTextureAsset(TEXT("Texture2D'/Game/Textures/T_Wood_WalnutThumbnail.T_Wood_WalnutThumbnail'"));

	BPTorch = TorchBPAsset.Object;
	
	SkeletalBlock = GrassBlockSkeletalMeshAsset.Object;
	SkeletalTorch = TorchSkeletalMeshAsset.Object;
	SkeletalPickaxe = PickaxeSkeletalMeshAsset.Object;
	StaticTorch = TorchStaticMeshAsset.Object;

	MaterialItem GrassItem, IronItem, SnowItem, CobbleItem, WoodItem, LeavesItem, TorchItem, DiamondPickaxeItem;

	DiamondPickaxeItem.MaterialType = EMaterial::DiamondPickaxe;
	DiamondPickaxeItem.Material = PickaxeMaterialAsset.Object;
	DiamondPickaxeItem.Thumbnail = GrassTextureAsset.Object;

	TorchItem.MaterialType = EMaterial::TorchMat;
	TorchItem.Material = TorchMaterialAsset.Object;
	TorchItem.Thumbnail = GrassTextureAsset.Object;

	GrassItem.MaterialType = EMaterial::Grass;
	GrassItem.Material = GrassBlockAsset.Object;
	GrassItem.Thumbnail = GrassTextureAsset.Object;

	IronItem.MaterialType = EMaterial::Iron;
	IronItem.Material = IronBlockAsset.Object;
	IronItem.Thumbnail = IronTextureAsset.Object;

	SnowItem.MaterialType = EMaterial::Snow;
	SnowItem.Material = SnowBlockAsset.Object;
	SnowItem.Thumbnail = SnowTextureAsset.Object;

	CobbleItem.MaterialType = EMaterial::Cobble;
	CobbleItem.Material = CobbleBlockAsset.Object;
	CobbleItem.Thumbnail = CobbleTextureAsset.Object;

	WoodItem.MaterialType = EMaterial::WoodOak;
	WoodItem.Material = WoodBlockAsset.Object;
	WoodItem.Thumbnail = WoodTextureAsset.Object;

	LeavesItem.MaterialType = EMaterial::Leaves;
	LeavesItem.Material = LeavesBlockAsset.Object;
	LeavesItem.Thumbnail = LeavesTextureAsset.Object;

	Materials.Add(DiamondPickaxeItem);
	Materials.Add(GrassItem);
	Materials.Add(IronItem);
	Materials.Add(SnowItem);
	Materials.Add(CobbleItem);
	Materials.Add(WoodItem);
	Materials.Add(LeavesItem);
	Materials.Add(TorchItem);
}

// Called when the game starts or when spawned
void AAssetsManager::BeginPlay() {
	Super::BeginPlay();
}

FName AAssetsManager::ConvertMaterialTypeToString(TEnumAsByte<EMaterial> SearchMaterialType) {
	switch(SearchMaterialType) {
		case EMaterial::Grass:
			return "Grass";
		case EMaterial::Snow:
			return "Snow";
		case EMaterial::Iron:
			return "Iron";
		case EMaterial::Cobble:
			return "Cobble";
		case EMaterial::WoodOak:
			return "WoodOak";
		case EMaterial::Leaves:
			return "Leaves";
		case EMaterial::TorchMat:
			return "Torch";
		case EMaterial::DiamondPickaxe:
			return "DiamondPickaxe";
		default:
			return "";
	}
}

TEnumAsByte<EMaterial> AAssetsManager::GetTypeByMaterial(const UMaterial* SearchMaterial) {
	for (int i = 0; i < Materials.Num(); i++) {
		if (Materials[i].Material == SearchMaterial) {
			return Materials[i].MaterialType;
		}
	}

	return Materials[0].MaterialType;
}

UMaterial* AAssetsManager::GetMaterialByType(const TEnumAsByte<EMaterial> SearchMaterialType) {
	for (int i = 0; i < Materials.Num(); i++) {
		if (Materials[i].MaterialType == SearchMaterialType) {
			return Materials[i].Material;
		}
	}

	return Materials[0].Material;
}

UTexture2D* AAssetsManager::GetThumbnailByMaterial(const UMaterial* SearchMaterial) {
	for (int i = 0; i < Materials.Num(); i++) {
		if (Materials[i].Material == SearchMaterial) {
			return Materials[i].Thumbnail;
		}
	}

	return Materials[0].Thumbnail;
}

UTexture2D* AAssetsManager::GetThumbnailByMaterial(const TEnumAsByte<EMaterial> SearchMaterialType) {
	return GetThumbnailByMaterial(GetMaterialByType(SearchMaterialType));
}

