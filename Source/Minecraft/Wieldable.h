// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Minecraft.h"
#include "GameFramework/Actor.h"
#include "Wieldable.generated.h"

UCLASS()
class MINECRAFT_API AWieldable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWieldable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ETool> ToolType;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMaterial> MaterialType;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* WieldableMesh;

	UPROPERTY(EditAnywhere)
	UShapeComponent* PickupTrigger;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* PickupThumbnail;

	bool bIsActive;

	UFUNCTION()
	void OnOverlap(class UPrimitiveComponent* Comp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnUsed();
	void Hide(bool bVis);
};
