// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MINECRAFT_API TreeGenerator
{
public:
	TreeGenerator();
	~TreeGenerator();

	static void GenerateTree(FVector, TMap<FString, bool>&, int, UInstancedStaticMeshComponent*, UInstancedStaticMeshComponent*);
};
