// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeGenerator.h"

TreeGenerator::TreeGenerator()
{
}

void TreeGenerator::GenerateTree(FVector Center, 
TMap<FString, bool> &ReservedTreePosMap, 
int VOXEL_SIZE, UInstancedStaticMeshComponent* TreeTrunkInstance, UInstancedStaticMeshComponent* LeavesInstance) {
    	// generate tree
	if (ReservedTreePosMap.Contains(FVector(Center.X, Center.Y, 0).ToString())) {
		return;
	}

	float StartX = Center.X - VOXEL_SIZE * 5;
	float StartY = Center.Y - VOXEL_SIZE * 5;

	for (int i = 0; i < 10; i++) {
		float X = StartX + i * VOXEL_SIZE;
		for (int j = 0; j < 10; j++) {
			float Y = StartY + j * VOXEL_SIZE;
			ReservedTreePosMap.Add(FVector(X, Y, 0).ToString());
		}
	}

	float TreeX = Center.X;
	float TreeY = Center.Y;
	float TreeZ = Center.Z + VOXEL_SIZE;

	// first level 

	for (int i = 0; i < 5; i++) {
		// ствол дерева
		FTransform TreeInstanceTransform;
		TreeInstanceTransform.SetLocation(FVector(TreeX, TreeY, TreeZ));
		TreeZ += VOXEL_SIZE;
		TreeTrunkInstance->AddInstance(TreeInstanceTransform);
	}

	TreeX -= 2 * VOXEL_SIZE;
	TreeY -= 2 * VOXEL_SIZE;

	// first level
	for (int g = 0; g < 5; g++) {
		float LocalTreeX = TreeX + VOXEL_SIZE * g;

		for (int h = 0; h < 5; h++) {
			float LocalTreeY = TreeY + VOXEL_SIZE * h;

			FTransform TreeInstanceTransform;
			TreeInstanceTransform.SetLocation(FVector(LocalTreeX, LocalTreeY, TreeZ));
			LeavesInstance->AddInstance(TreeInstanceTransform);
		}
	}

	TreeX += VOXEL_SIZE;
	TreeY += VOXEL_SIZE;
	TreeZ += VOXEL_SIZE;

	// second lvl
	for (int g = 0; g < 3; g++) {
		float LocalTreeX = TreeX + VOXEL_SIZE * g;

		for (int h = 0; h < 3; h++) {
			float LocalTreeY = TreeY + VOXEL_SIZE * h;

			FTransform TreeInstanceTransform;
			TreeInstanceTransform.SetLocation(FVector(LocalTreeX, LocalTreeY, TreeZ));
			LeavesInstance->AddInstance(TreeInstanceTransform);
		}
	}
}
