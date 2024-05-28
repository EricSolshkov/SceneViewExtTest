﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "AcThermalManager.h"

#include "Components/BillboardComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values for this component's properties
UAcThermalManager::UAcThermalManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SurfaceTemperature = 64.0;

	ThermalRenderingEnabled = false;

	TemperatureHighCut = 40;

	TemperatureLowCut = 20;

	HeatSources.Empty();
}

void SaveOriginalMaterials(AActor* MyActor, UAcThermalManager* ATM)
{
	ATM->MeshComponent = MyActor->FindComponentByClass<UMeshComponent>();
	if (ATM->MeshComponent)
	{
		ATM->OriginalMaterials = ATM->MeshComponent->GetMaterials();
	}
}

// Called when the game starts
void UAcThermalManager::BeginPlay()
{
	Super::BeginPlay();
	
	ThermalMaterialInstance = UMaterialInstanceDynamic::Create(ThermalMaterialPtr::Get(), this);
	
	// Store original mesh material.
	const auto MyActor = GetOwner();

	if (MyActor)
	{
		SaveOriginalMaterials(MyActor, this);
	}
	
	if (ThermalMaterialInstance)
	{
		SetTemperatureHighCut(TemperatureHighCut);
		SetTemperatureLowCut(TemperatureLowCut);
	}
}


// Called every frame
void UAcThermalManager::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update Thermal Material Temperature Parameter.
	if (ThermalMaterialInstance && ThermalRenderingEnabled)
	{
		ThermalMaterialInstance->SetScalarParameterValue(FName("SurfaceTemperature"), SurfaceTemperature);
	}
}

void UAcThermalManager::SetTemperatureHighCut(const float NewTemperature)
{
	TemperatureHighCut = NewTemperature;
	if(ThermalMaterialInstance)
	{
		MeshComponent->SetScalarParameterValueOnMaterials(FName("HighCut"), NewTemperature);
	}
}

float UAcThermalManager::GetTemperatureHighCut()
{
	return TemperatureHighCut;
}

void UAcThermalManager::SetTemperatureLowCut(const float NewTemperature)
{
	TemperatureLowCut = NewTemperature;
	if(ThermalMaterialInstance)
	{
		MeshComponent->SetScalarParameterValueOnMaterials(FName("LowCut"), NewTemperature);
	}
}

float UAcThermalManager::GetTemperatureLowCut()
{
	return TemperatureLowCut;
}

void UAcThermalManager::EnableThermalRendering()
{
	if (!ThermalRenderingEnabled && ThermalMaterialInstance)
	{
		ThermalRenderingEnabled = true;
		
		for (int i = 0; i < OriginalMaterials.Num(); ++i)
		{
			MeshComponent->SetMaterial(i, ThermalMaterialInstance);
		}
	}
	auto name = GetOwner()->GetName();

	float st, lc, hc;
	ThermalMaterialInstance->GetScalarParameterValue(FName("SurfaceTemperature"), st);
	ThermalMaterialInstance->GetScalarParameterValue(FName("LowCut"), lc);
	ThermalMaterialInstance->GetScalarParameterValue(FName("HighCut"), hc);

	for (auto IMat : MeshComponent->GetMaterials())
	{
		auto Mat = Cast<UMaterialInstanceDynamic, UMaterialInterface>(IMat);
		Mat->GetScalarParameterValue(FName("SurfaceTemperature"), st);
		Mat->GetScalarParameterValue(FName("LowCut"), lc);
		Mat->GetScalarParameterValue(FName("HighCut"), hc);
	}
	
	int b = 0;
}

void UAcThermalManager::DisableThermalRendering()
{
	if (ThermalRenderingEnabled)
	{
		for (int i = 0; i < OriginalMaterials.Num(); ++i)
		{
			MeshComponent->SetMaterial(i, OriginalMaterials[i]);
		}
		ThermalRenderingEnabled = false;
	}
}

bool UAcThermalManager::GetThermalRenderingStatus()
{
	return ThermalRenderingEnabled;
}

void UAcThermalManager::AppendHeatSourcesMeta(TArray<FHeatSourceMeta>& Container)
{
	for(auto Hr : HeatSources)
	{
		if(IsValid(Hr))
			Container.Add(Hr->GetMeta(GetTemperatureLowCut(), GetTemperatureHighCut()));
	}
}

UAcThermalManager* UAcThermalManager::Create(AActor* Actor, float Temperature, bool Enabled)
{
	// Check to avoid creating redundant ThermalManager for invisible actors.
	auto mesh = Actor->FindComponentByClass<UMeshComponent>();
	if (!mesh) return nullptr;

	// Check to avoid creating ThermalManager for billboard and text renderers.
	// Due to ThermalVisionCS is working on each pixel and lack stencil check, billboard and text pixels will be mapped.
	// this avoidance will NOT achieve its goal.
	auto bill = Cast<UBillboardComponent, UMeshComponent>(mesh);
	if (bill) return nullptr;
	auto text = Cast<UTextRenderComponent, UMeshComponent>(mesh);
	if (text) return nullptr;

    // Check to avoid creating duplicate ThermalManager
	auto atm = Actor->FindComponentByClass<UAcThermalManager>();
	if (atm) return atm;

	FTransform Transform = UKismetMathLibrary::MakeTransform(
		FVector(0.0, 0.0, 0.0),
		FRotator(0.0, 0.0, 0.0),
		FVector(1.0, 1.0, 1.0));
	const auto Component = Actor->AddComponentByClass(StaticClass(), false, Transform, false);
	const auto ThermalManager = Cast<UAcThermalManager, UActorComponent>(Component);

	// Here we require and assume TargetThermalManagerComponent is not nullptr.
	// if this assume fails program crashes here, check then.
	ThermalManager->ThermalMaterialInstance = UMaterialInstanceDynamic::Create(ThermalMaterialPtr::Get(), ThermalManager);
	SaveOriginalMaterials(Actor, ThermalManager);
	ThermalManager->SetSurfaceTemperature(Temperature);
	if (Enabled) ThermalManager->EnableThermalRendering();
	
	return ThermalManager;
}

void UAcThermalManager::AddHeatSource()
{
	// CreateDefaultSubObject works only in constructors.
	// CreateNewObj can be called runtime.
	const auto Hr = Cast<AHeatSource, AActor>(GetWorld()->SpawnActor(AHeatSource::StaticClass()));
	HeatSources.Add(Hr);
	Hr->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
}





