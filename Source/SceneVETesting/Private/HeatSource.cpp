// Fill out your copyright notice in the Description page of Project Settings.
#include "HeatSource.h"

#include "AcThermalManager.h"
#include "DrawDebugHelpers.h"
#include "GeometryCollection/GeometryCollectionAlgo.h"


AHeatSourceBase::AHeatSourceBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Actor contains no default SC, if need transform, create sc & set it as root.
	USceneComponent* SC = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SC);
	Temperature = 100;
	Shape = EHeatSourceShapeType::None;
}

AHeatSourceBase::AHeatSourceBase(const FObjectInitializer& ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Actor contains no default SC, if need transform, create sc & set it as root.
	USceneComponent* SC = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SC);
	Temperature = 100;
	Shape = EHeatSourceShapeType::None;
}

void AHeatSourceBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if WITH_EDITOR
	if (!GWorld->HasBegunPlay() || this->IsSelected())
	{
		DrawDebugShape();
	}
#endif
}

void AHeatSourceBase::DrawDebugShape()
{
	return;
}

ASphereHeatSource::ASphereHeatSource()
{
	Super();
	Shape = EHeatSourceShapeType::Sphere;
	Radius = 256;
}

ASphereHeatSource::ASphereHeatSource(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Shape = EHeatSourceShapeType::Sphere;
	Radius = 256;
}


// Called every frame
void ASphereHeatSource::DrawDebugShape()
{
	DrawDebugSphere(
		GetWorld(),
		GetTransform().GetLocation(),
		Radius,
		32,
		FColor::Yellow,
		false,
		0);
}

FSphereMeta ASphereHeatSource::GetMeta() const
{
	FSphereMeta Meta;
	Meta.WorldToLocal = GetTransform().ToMatrixNoScale().Inverse().GetTransposed();
	Meta.Radius = Radius;
	Meta.Temperature = Temperature;
	Meta.Thickness = Thickness;
	return Meta;
}

ABoxHeatSource::ABoxHeatSource()
{
	Super();
	Shape = EHeatSourceShapeType::Box;
	XYZ = FVector(256, 256, 256);
}

ABoxHeatSource::ABoxHeatSource(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Shape = EHeatSourceShapeType::Box;
	XYZ = FVector(256, 256, 256);
}

void ABoxHeatSource::DrawDebugShape()
{
	DrawDebugBox(
		GetWorld(),
		GetTransform().GetLocation(),
		XYZ,
		GetTransform().GetRotation(),
		FColor::Yellow,
		false,
		0);
}

FBoxMeta ABoxHeatSource::GetMeta() const
{
	FBoxMeta Meta;
	Meta.WorldToLocal = GetTransform().ToMatrixNoScale().Inverse().GetTransposed();
	Meta.XYZ = XYZ;
	Meta.Temperature = Temperature;
	Meta.Thickness = Thickness;
	return Meta;
}

ACapsuleHeatSource::ACapsuleHeatSource()
{
	Super();
	Shape = EHeatSourceShapeType::Sphere;
	Radius = 50;
	HalfHeight = 50;
}

ACapsuleHeatSource::ACapsuleHeatSource(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Shape = EHeatSourceShapeType::Capsule;
	Radius = 50;
	HalfHeight = 50;
}

void ACapsuleHeatSource::DrawDebugShape()
{
	DrawDebugCapsule(
		GetWorld(),
		GetTransform().GetLocation(),
		HalfHeight,
		Radius,
		GetTransform().GetRotation(),
		FColor::Yellow,
		false,
		0);
}

FCapsuleMeta ACapsuleHeatSource::GetMeta() const
{
	FCapsuleMeta Meta;
	Meta.WorldToLocal = GetTransform().ToMatrixNoScale().Inverse().GetTransposed();
	Meta.Temperature = Temperature;
	Meta.Radius = Radius;
	Meta.HalfHeight = HalfHeight < Radius ? Radius : HalfHeight;
	Meta.Thickness = Thickness;
	return Meta;
}
