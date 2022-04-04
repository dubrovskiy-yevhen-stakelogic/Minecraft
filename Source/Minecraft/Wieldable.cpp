// Fill out your copyright notice in the Description page of Project Settings.

#include "Wieldable.h"
#include "MinecraftCharacter.h"

// Sets default values
AWieldable::AWieldable() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Counter = 1;

	WieldableMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WieldableMesh"));
	PickupTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupTrigger"));

	PickupTrigger->SetGenerateOverlapEvents(true);
	PickupTrigger->OnComponentBeginOverlap.AddDynamic(this, &AWieldable::OnOverlap);
	PickupTrigger->AttachToComponent(WieldableMesh, FAttachmentTransformRules::KeepRelativeTransform);

	MaterialType = EMaterial::NoneMaterial;
	ToolType = ETool::Unarmed;

	bIsActive = true;
}

// Called when the game starts or when spawned
void AWieldable::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AWieldable::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FRotator rotation = WieldableMesh->GetComponentRotation();
	rotation.Yaw += 1.f;
	WieldableMesh->SetRelativeRotation(rotation);
}

void AWieldable::OnOverlap(UPrimitiveComponent*, class AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&) {
	if (bIsActive) {
		AMinecraftCharacter* Character = Cast<AMinecraftCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		Character->FP_Gun->SetSkeletalMesh(WieldableMesh->SkeletalMesh);
		Character->AddItemToInventory(this);

		Destroy();
		// Hide(true);
	}
}

void AWieldable::Hide(bool bVis) {
	WieldableMesh->SetVisibility(!bVis);
	bIsActive = !bVis;
}

void AWieldable::OnUsed() {
	Destroy();
}
