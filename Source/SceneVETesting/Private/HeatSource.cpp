// Fill out your copyright notice in the Description page of Project Settings.
#include "HeatSource.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "GenericPlatform/GenericPlatformCrashContext.h"

FHeatSourceMeta::FHeatSourceMeta()
{
	Center = FVector(0,0,0);
	Radius = 0;
	Color = FVector(0,0,0);
}

FHeatSourceMeta::FHeatSourceMeta(const FVector& iCenter, const FVector& iColor, float iRadius)
{
	Center = iCenter;
	Radius = iRadius;
	Color = iColor;
}

// Sets default values
AHeatSource::AHeatSource()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Temperature = 20;
}

void AHeatSource::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		GetTransform().GetLocation(),
		CurrentSize,
		32,
		FColor(255,255,255));
#endif
}

// Called when the game starts or when spawned
void AHeatSource::BeginPlay()
{
	Super::BeginPlay();
	SpawnSize = 256;
}

// Called every frame
void AHeatSource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CurrentSize = SpawnSize;
	
	if (ShapeType == EHeatSourceShapeType::Sphere)
	{
		DrawDebugSphere(GetWorld(),
		          GetTransform().GetLocation(),
		          CurrentSize,
		          32,
		          FColor(255, 255, 0));
	}
}

FHeatSourceMeta AHeatSource::GetMeta()
{
	FHeatSourceMeta Meta = FHeatSourceMeta(GetTransform().GetLocation(), FVector(1,1,1), CurrentSize);
	return Meta;
}

