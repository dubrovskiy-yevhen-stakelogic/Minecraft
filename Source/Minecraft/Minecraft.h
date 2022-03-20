// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "CoreMinimal.h"

UENUM()
enum EMaterial {
	NoneMaterial = 1,
	Wooden = 2,
	Stone = 4,
	Iron = 6,
	Diamond = 8,
	Golden = 12
};

UENUM()
enum ETool {
	Unarmed,
	Pickaxe,
	Axe,
	Shovel,
	Sword
};
