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


	UPROPERTY()
	USkeletalMesh* SkeletalMeshAsset;

	TArray<MaterialItem> Materials;

	TEnumAsByte<EMaterial> GetTypeByMaterial(const UMaterial*);
	UMaterial* GetMaterialByType(TEnumAsByte<EMaterial>);
	static FName ConvertMaterialTypeToString(TEnumAsByte<EMaterial>);

	UTexture2D* GetThumbnailByMaterial(const UMaterial*);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
