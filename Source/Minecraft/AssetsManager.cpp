// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetsManager.h"

// Sets default values
AAssetsManager::AAssetsManager() {
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UMaterial> GrassBlockAsset(TEXT("Material'/Game/Assets/Materials/M_Grass.M_Grass'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> IronBlockAsset(TEXT("Material'/Game/Assets/Materials/M_Iron.M_Iron'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> SnowBlockAsset(TEXT("Material'/Game/Assets/Materials/M_Snow.M_Snow'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> CobbleBlockAsset(TEXT("Material'/Game/Assets/Materials/M_Cobble.M_Cobble'"));

	static ConstructorHelpers::FObjectFinder<UTexture2D> GrassTexturehAsset(TEXT("Texture2D'/Game/Assets/Textures/T_GrassThumbnail.T_GrassThumbnail'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IronTexturehAsset(TEXT("Texture2D'/Game/Assets/Textures/T_IronThumbnail.T_IronThumbnail'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> SnowTexturehAsset(TEXT("Texture2D'/Game/Assets/Textures/T_SnowThumbnail.T_SnowThumbnail'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> CobbleTexturehAsset(TEXT("Texture2D'/Game/Assets/Textures/T_CobbleThumbnail.T_CobbleThumbnail'"));

	MaterialItem GrassItem, IronItem, SnowItem, CobbleItem;

	GrassItem.MaterialType = EMaterial::Grass;
	GrassItem.Material = GrassBlockAsset.Object;
	GrassItem.Thumbnail = GrassTexturehAsset.Object;

	IronItem.MaterialType = EMaterial::Iron;
	IronItem.Material = IronBlockAsset.Object;
	IronItem.Thumbnail = IronTexturehAsset.Object;

	SnowItem.MaterialType = EMaterial::Snow;
	SnowItem.Material = SnowBlockAsset.Object;
	SnowItem.Thumbnail = SnowTexturehAsset.Object;

	CobbleItem.MaterialType = EMaterial::Cobble;
	CobbleItem.Material = CobbleBlockAsset.Object;
	CobbleItem.Thumbnail = CobbleTexturehAsset.Object;

	Materials.Add(GrassItem);
	Materials.Add(IronItem);
	Materials.Add(SnowItem);
	Materials.Add(CobbleItem);
}

// Called when the game starts or when spawned
void AAssetsManager::BeginPlay() {
	Super::BeginPlay();
}

FName AAssetsManager::ConvertMaterialTypeToString(TEnumAsByte<EMaterial> SearchMaterialType) {
	switch(SearchMaterialType) {
		case EMaterial::Grass:
			return "Grass";
			break;
		case EMaterial::Snow:
			return "Snow";
			break;
		case EMaterial::Iron:
			return "Iron";
			break;
		case EMaterial::Cobble:
			return "Cobble";
			break;
		default:
			return "";
	}
}

TEnumAsByte<EMaterial> AAssetsManager::GetTypeByMaterial(UMaterial* SearchMaterial) {
	for (int i = 0; i < Materials.Num(); i++) {
		if (Materials[i].Material == SearchMaterial) {
			return Materials[i].MaterialType;
		}
	}

	return Materials[0].MaterialType;
}

UMaterial* AAssetsManager::GetMaterialByType(TEnumAsByte<EMaterial> SearchMaterialType) {
	for (int i = 0; i < Materials.Num(); i++) {
		if (Materials[i].MaterialType == SearchMaterialType) {
			return Materials[i].Material;
		}
	}

	return Materials[0].Material;
}

UTexture2D* AAssetsManager::GetThumbnailByMaterial(UMaterial* SearchMaterial) {
	for (int i = 0; i < Materials.Num(); i++) {
		if (Materials[i].Material == SearchMaterial) {
			return Materials[i].Thumbnail;
		}
	}

	return Materials[0].Thumbnail;
}

