// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minecraft.h"
#include "GameFramework/Actor.h"
#include "AssetsManager.generated.h"

struct MaterialItem {
	UMaterial* Material;
	TEnumAsByte<EMaterial> MaterialType;
	UTexture2D* Thumbnail;
};

UCLASS()
class MINECRAFT_API AAssetsManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAssetsManager();
	
	static USkeletalMesh* SkeletalBlock;
	static USkeletalMesh* SkeletalTorch;
	static USkeletalMesh* SkeletalPickaxe;
	static UStaticMesh* StaticTorch;
	static UBlueprint* BPTorch;

	static TArray<MaterialItem> Materials;

	static TEnumAsByte<EMaterial> GetTypeByMaterial(const UMaterial*);
	static UMaterial* GetMaterialByType(const TEnumAsByte<EMaterial>);
	static FName ConvertMaterialTypeToString(TEnumAsByte<EMaterial>);
	static UTexture2D* GetThumbnailByMaterial(const UMaterial*);
	static UTexture2D* GetThumbnailByMaterial(const TEnumAsByte<EMaterial>);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
