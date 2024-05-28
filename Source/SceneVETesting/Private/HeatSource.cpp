// Fill out your copyright notice in the Description page of Project Settings.
#include "HeatSource.h"

#include "AcThermalManager.h"
#include "DrawDebugHelpers.h"
#include "BaseGizmos/TransformGizmo.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "GenericPlatform/GenericPlatformCrashContext.h"

FHeatSourceMeta::FHeatSourceMeta()
{
	Center = FVector(0,0,0);
	Radius = 0;
	Temperature = 0.0f;
}

FHeatSourceMeta::FHeatSourceMeta(const FVector& iCenter, float iRadius, float iTemperature)
{
	Center = iCenter;
	Radius = iRadius;
	Temperature = iTemperature;
}

// Sets default values
AHeatSource::AHeatSource() : Super()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Actor contains no default SC, if need transform, create sc & set it as root.
	USceneComponent* SC = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SC);
	Temperature = 20;
}

AHeatSource::AHeatSource(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* SC = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SC);
	Temperature = 20;
}

void AHeatSource::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	if (ShapeType == EHeatSourceShapeType::Sphere)
	{
		DrawDebugSphere(GetWorld(),
				  GetTransform().GetLocation(),
				  CurrentSize,
				  32,
				  FColor(255, 255, 0),
				  true);
	}
}
#endif

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
}

FHeatSourceMeta AHeatSource::GetMeta(float LowCut, float HighCut) const
{
	HighCut = (HighCut > LowCut + 0.1f) ? HighCut : (LowCut + 0.1f);
	FHeatSourceMeta Meta = FHeatSourceMeta(GetTransform().GetLocation(), CurrentSize, (Temperature - LowCut) / (HighCut - LowCut));
	return Meta;
}

