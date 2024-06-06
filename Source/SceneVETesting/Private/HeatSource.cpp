// Fill out your copyright notice in the Description page of Project Settings.
#include "HeatSource.h"

#include "AcThermalManager.h"
#include "DrawDebugHelpers.h"
#include "BaseGizmos/TransformGizmo.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "GenericPlatform/GenericPlatformCrashContext.h"


FHeatSourceMeta::FHeatSourceMeta()
{
	Center = FVector(0, 0, 0);
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
	Temperature = 100;
}

// Called when the game starts or when spawned
void AHeatSource::BeginPlay()
{
	Super::BeginPlay();
	CurrentSize = SpawnSize;
}

// Called every frame
void AHeatSource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if WITH_EDITOR
	if(!GWorld->HasBegunPlay())
	{
		if (ShapeType == EHeatSourceShapeType::Sphere)
		{
			auto aaa = GetTransform().GetLocation();
			DrawDebugSphere(
				GetWorld(),
				aaa,
				CurrentSize,
				32,
				FColor::Yellow,
				false,
				0);
		}
	}
#endif
}

FHeatSourceMeta AHeatSource::GetMeta(float LowCut, float HighCut) const
{
	HighCut = (HighCut > LowCut + 0.1f) ? HighCut : (LowCut + 0.1f);
	FHeatSourceMeta Meta = FHeatSourceMeta(
		GetTransform().GetLocation(),
		CurrentSize,
		Temperature);
	return Meta;
}



#if WITH_EDITOR
bool AHeatSource::ShouldTickIfViewportsOnly() const
{
	return WITH_EDITOR;
}
#endif
