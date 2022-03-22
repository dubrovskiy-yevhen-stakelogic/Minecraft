// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinecraftCharacter.h"
#include "MinecraftProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMinecraftCharacter

AMinecraftCharacter::AMinecraftCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	Reach = 250.f;
}

void AMinecraftCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMinecraftCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AMinecraftCharacter::Throw);

	PlayerInputComponent->BindAction("InventoryUp", IE_Pressed, this, &AMinecraftCharacter::MoveUpInventorySlot);
	PlayerInputComponent->BindAction("InventoryDown", IE_Released, this, &AMinecraftCharacter::MoveDownInventorySlot);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

		// Bind fire event
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMinecraftCharacter::OnHit);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMinecraftCharacter::EndHit);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMinecraftCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMinecraftCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMinecraftCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMinecraftCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMinecraftCharacter::LookUpAtRate);
}

void AMinecraftCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMinecraftCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		// OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AMinecraftCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AMinecraftCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AMinecraftCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMinecraftCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMinecraftCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMinecraftCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AMinecraftCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMinecraftCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AMinecraftCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AMinecraftCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void AMinecraftCharacter::DestroyChunksOutsideRenderRange() {
	FVector PlayerLocation = FirstPersonCameraComponent->GetComponentLocation();

	for (int g = 0; g < VisibleChunks.Num(); g++) {
		FVector CurrentChunkLoc = VisibleChunks[g]->GetActorLocation();
		float Distance = FVector::Dist(FVector(PlayerLocation.X, PlayerLocation.Y, 0), CurrentChunkLoc);

		if ( Distance > RENDER_RANGE_TILES * AChunk::CHUNK_SIZE_XY ) {
			// VisibleChunks[g]->Destroy();
			AChunk* SpawnedChunk = SpawnedChunksMap[CurrentChunkLoc.ToString()];
			if (SpawnedChunk) {
				SpawnedChunk->SetVisibility(false);
			} 
			
			VisibleChunks.RemoveAt(g);
			--g;
		}
	}
}

void AMinecraftCharacter::SpawnNewChunks() {
	FVector PlayerLocation = FirstPersonCameraComponent->GetComponentLocation();

	for (int i = RENDER_RANGE_TILES * -1; i < RENDER_RANGE_TILES; i++) {
		for (int j = RENDER_RANGE_TILES * -1; j < RENDER_RANGE_TILES; j++) {
			FVector ChunkLoc = FVector((i + LastPlayerChunkCoord.X) * AChunk::CHUNK_SIZE_XY, ( j + LastPlayerChunkCoord.Y) * AChunk::CHUNK_SIZE_XY, 0);
			FRotator Rotation(0.0f, 0.0f, 0.0f);		

			bool IsFound = SpawnedChunksMap.Contains(ChunkLoc.ToString());	

			float Distance = FVector::Dist(FVector(PlayerLocation.X, PlayerLocation.Y, 0), ChunkLoc);

			// FString toString = FString::Printf(TEXT("VectorSize=%d"), ChunkLoc.Size());
			// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, toString);

			bool IsInRange = Distance <= RENDER_RANGE_TILES * AChunk::CHUNK_SIZE_XY;

			if (IsFound && IsInRange) {
				SpawnedChunksMap[ChunkLoc.ToString()]->SetVisibility(true);
			} 

			bool IsReadyToAddNewChunk = !IsFound && IsInRange;

			if (IsReadyToAddNewChunk) {
				AChunk* NewChunk = GetWorld()->SpawnActor<AChunk>(FVector(ChunkLoc.X, ChunkLoc.Y, 0),  Rotation);
				VisibleChunks.Add(NewChunk);
				SpawnedChunksMap.Add(ChunkLoc.ToString(), NewChunk);
			}
		}
	}
}

FIntVector AMinecraftCharacter::GetPlayerChunkCoordinates() {
	FVector Location = FirstPersonCameraComponent->GetComponentLocation();

	int X = FMath::RoundToInt(Location.X / AChunk::CHUNK_SIZE_XY);
	int Y = FMath::RoundToInt(Location.Y / AChunk::CHUNK_SIZE_XY);

	return FIntVector(X, Y, 0);
}

void AMinecraftCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	DestroyChunksOutsideRenderRange();

	FIntVector CurrentChunkCoord = GetPlayerChunkCoordinates();

	if (CurrentChunkCoord.X == LastPlayerChunkCoord.X && CurrentChunkCoord.Y == LastPlayerChunkCoord.Y) {
		return;
	}

	LastPlayerChunkCoord = CurrentChunkCoord;

	SpawnNewChunks();

	CheckForBlocks();
}

void AMinecraftCharacter::CheckForBlocks() {
	FHitResult LinetraceHit;
	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
	FVector EndTrace = (FirstPersonCameraComponent->GetForwardVector() * Reach ) + StartTrace;

	FCollisionQueryParams CQP;

	CQP.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(LinetraceHit, StartTrace, EndTrace, ECollisionChannel::ECC_WorldDynamic, CQP);

	ABlock* PotentialBlock = Cast<ABlock>(LinetraceHit.GetActor());
		
	// FString toString = FString::Printf(TEXT("VectorSize=%d"), ChunkLoc.Size());
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, toString);


	if (PotentialBlock != CurrentBlock && CurrentBlock != nullptr) {
		CurrentBlock->ResetBlock();
	}

	if (PotentialBlock == NULL) {
		CurrentBlock = nullptr;
		return;
	}

	if (CurrentBlock != nullptr && !bIsBreaking) {
		CurrentBlock->ResetBlock();
	}

	CurrentBlock = PotentialBlock;
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *CurrentBlock->GetName());
}

void AMinecraftCharacter::PlayHitAnim() {
	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AMinecraftCharacter::OnHit() {
	PlayHitAnim();

	if (CurrentBlock != nullptr) {
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ZASHEL!!!"));
		bIsBreaking = true;

		float TimeBetweenBreaks = (( CurrentBlock->Resistance) / 100.f) / 2;

		GetWorld()->GetTimerManager().SetTimer(BlockBreakingHandle, this, &AMinecraftCharacter::BreakBlock, TimeBetweenBreaks, true);
		GetWorld()->GetTimerManager().SetTimer(HitAnimHandle, this, &AMinecraftCharacter::PlayHitAnim, 0.4, true);
	}
}

void AMinecraftCharacter::EndHit() {
	GetWorld()->GetTimerManager().ClearTimer(BlockBreakingHandle);
	GetWorld()->GetTimerManager().ClearTimer(HitAnimHandle);

	bIsBreaking = false;

	if (CurrentBlock != nullptr) {
		CurrentBlock->ResetBlock();
	}
}

void AMinecraftCharacter::BreakBlock() {
	if (bIsBreaking && CurrentBlock != nullptr && !CurrentBlock->IsPendingKill()) {
		CurrentBlock->Break();
	}
}

int32 AMinecraftCharacter::GetCurrentInventorySlot() {
	return CurrentInventorySlot;
}

bool AMinecraftCharacter::AddItemToInventory(AWieldable* Item) {
	if (Item != NULL) {
		const int32 AvailableSlot = Inventory.Find(nullptr);

		if (AvailableSlot != INDEX_NONE) {
			Inventory[AvailableSlot] = Item;
			UpdateWieldedItem();
			return true;
		}
	}

	return false;
}

UTexture2D* AMinecraftCharacter::GetThumbnailAtInventorySlot(uint8 Slot) {
	if (Inventory[Slot] != NULL) {
		return Inventory[Slot]->PickupThumbnail;
	}

	return nullptr;
}

void AMinecraftCharacter::MoveUpInventorySlot() {
	CurrentInventorySlot = FMath::Abs(( CurrentInventorySlot + 1 ) % NUM_OF_INVENTORY_SLOTS);
	UpdateWieldedItem();
}

void AMinecraftCharacter::MoveDownInventorySlot() {
	if (CurrentInventorySlot == 0) {
		CurrentInventorySlot = 9;
		UpdateWieldedItem();
		return;
	}

	CurrentInventorySlot = FMath::Abs(( CurrentInventorySlot - 1 ) % NUM_OF_INVENTORY_SLOTS);
	UpdateWieldedItem();
}

void AMinecraftCharacter::UpdateWieldedItem() {
	if (GetCurrentWieldedItem() != NULL) {
 		FP_Gun->SetSkeletalMesh(GetCurrentWieldedItem()->WieldableMesh->SkeletalMesh);
	} else {
		FP_Gun->SetSkeletalMesh(NULL);
	}
}

AWieldable* AMinecraftCharacter::GetCurrentWieldedItem() {
	if (Inventory[CurrentInventorySlot] != NULL) {
		return Inventory[CurrentInventorySlot];
	}

	return NULL;
}

void AMinecraftCharacter::Throw() {
	// get the currently wielded item

	AWieldable* ItemToThrow = GetCurrentWieldedItem();

	FHitResult LinetraceHit;

	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
	FVector EndTrace = (FirstPersonCameraComponent->GetForwardVector() * Reach ) + StartTrace;

	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(LinetraceHit, StartTrace, EndTrace, ECollisionChannel::ECC_WorldDynamic, CQP);

	FVector DropLocation = EndTrace;

	if (LinetraceHit.GetActor() != NULL) {
		DropLocation = (LinetraceHit.ImpactPoint + 20.f);
	}

	if (ItemToThrow != NULL) {
		UWorld* const World = GetWorld();
		if (World != NULL) {
			ItemToThrow->SetActorLocationAndRotation(DropLocation, FRotator::ZeroRotator);
			ItemToThrow->Hide(false);
			Inventory[CurrentInventorySlot] = NULL;
		}
	}

	UpdateWieldedItem();
}
