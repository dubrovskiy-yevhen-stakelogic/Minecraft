// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Minecraft.h"
#include "GameFramework/Character.h"
#include "Chunk.h"
#include "Wieldable.h"
#include "MinecraftCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

struct ChunkEntry {
	AChunk* Chunk;
	FVector ChunkLoc;
};

UCLASS(config=Game)
class AMinecraftCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* L_MotionController;

public:
	AMinecraftCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;

	/* Get the current inventory slot for the hud  */
	UFUNCTION(BlueprintPure, Category = HUD)
	int32 GetCurrentInventorySlot() const;

	/* Get the current inventory slot for the hud  */
	UFUNCTION(BlueprintPure, Category = Inventory)
	int32 GetCurrentInventoryCounter(uint8 Slot);

	/* Adds an item to our inventory */
	UFUNCTION(BlueprintPure, Category = Inventory)
	bool AddItemToInventory(AWieldable* Item);

	/* Gets the thumbnail for a given item */
	UFUNCTION(BlueprintPure, Category = Inventory)
	UTexture2D* GetThumbnailAtInventorySlot(uint8 Slot);

	TEnumAsByte<ETool> ToolType;
	TEnumAsByte<EMaterial> MaterialType;

protected:
	
	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

private:
	FTerrainGeneratorWorker* FWorker;

	// void RenderVisibleChunks(TArray<ChunkEntry> VisibleChunks);
	const int RENDER_RANGE_TILES = 6;
	// const int RENDER_RANGE = 500;

	void DestroyChunksOutsideRenderRange();

	void SpawnNewChunks();
	FIntVector GetPlayerChunkCoordinates() const;

	const int32 NUM_OF_INVENTORY_SLOTS = 10;

	FIntVector LastPlayerChunkCoord = FIntVector(-1, -1, 0);

	int CurrentInventorySlot;

	bool bIsBreaking;

	void UpdateWieldedItem();

	TMap<FString, BlockInfo>* Blocks;

	AWieldable* GetCurrentWieldedItem();

	void MoveUpInventorySlot();
	void MoveDownInventorySlot();

	void Throw();
	void Place();
	void OnHit();
	void EndHit();

	void PlayHitAnim() const;

	/* */
	void CheckForBlocks();

	void BreakBlock() const;

	UPROPERTY()
	AChunk* CurrentChunk;
	FVector_NetQuantize HitLocation;

	float Reach;

	FTimerHandle BlockBreakingHandle;
	FTimerHandle HitAnimHandle;

	UPROPERTY(EditAnywhere)
	TArray<AWieldable*> Inventory;

	TArray<ChunkEntry> VisibleChunks;
	TMap<FString, AChunk*>* SpawnedChunksMap;
public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

