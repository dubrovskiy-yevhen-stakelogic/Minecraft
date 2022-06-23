// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "CoreMinimal.h"

UENUM()
enum EMaterial {
	NoneMaterial,
	Grass,
	WoodOak,
	Leaves,
	Wooden,
	Cobble ,
	Iron,
	Snow,
	Diamond,
	Golden,
	TorchMat,
	DiamondPickaxe
};

UENUM()
enum ETool {
	Unarmed,
	Pickaxe,
	Axe,
	Shovel,
	Sword,
	Block,
	Torch
};

struct BlockInfo {
	int Neighbors;
	FVector Position;
	EMaterial MaterialType;
	bool IsBottom;
};

struct VisibleBlock  {
	FVector Position;
	EMaterial MaterialType;
};
