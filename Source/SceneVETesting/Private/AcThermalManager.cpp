// Fill out your copyright notice in the Description page of Project Settings.


#include "AcThermalManager.h"
#include "Materials/MaterialInstanceDynamic.h"



// Sets default values for this component's properties
UAcThermalManager::UAcThermalManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SurfaceTemperature = 64.0;

	ThermalRenderingEnabled = false;
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
	
	// Store oringinal mesh material.
	auto MyActor = GetOwner();

	if (MyActor)
	{
		SaveOriginalMaterials(MyActor, this);
	}
}


// Called every frame
void UAcThermalManager::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update Thermal Material Temperature Parameter.
	if (ThermalMaterialInstance)
	{
		MeshComponent->SetScalarParameterValueOnMaterials(FName("SurfaceTemperature"), SurfaceTemperature);
	}
}

void UAcThermalManager::EnableThermalRendering()
{
	if (!ThermalRenderingEnabled && OriginalMaterials.Num() != 0 && ThermalMaterialInstance)
	{
		for (int i = 0; i < OriginalMaterials.Num(); ++i)
		{
			MeshComponent->SetMaterial(i, ThermalMaterialInstance);
		}
		ThermalRenderingEnabled = true;
	}
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
		Container.Add(Hr->GetMeta());
	}
}

UAcThermalManager* UAcThermalManager::Create(AActor* Actor, float Temperature, bool Enabled)
{
	// Check to avoid creating redundant ThermalManager for invisible actors.
	auto mesh = Actor->FindComponentByClass<UMeshComponent>();
	if (!mesh) return nullptr;

    // Check to avoid creating duplicate ThermalManager
	auto atm = Actor->FindComponentByClass<UAcThermalManager>();
	if (atm) return nullptr;

	FTransform Transform = UKismetMathLibrary::MakeTransform(
		FVector(0.0, 0.0, 0.0),
		FRotator(0.0, 0.0, 0.0),
		FVector(1.0, 1.0, 1.0));
	auto ac = Actor->AddComponentByClass(StaticClass(), false, Transform, false);
	auto attached = Cast<UAcThermalManager, UActorComponent>(ac);
	

	if (attached)
	{
		attached->ThermalMaterialInstance = UMaterialInstanceDynamic::Create(ThermalMaterialPtr::Get(), attached);
		SaveOriginalMaterials(Actor, attached);
		attached->SetSurfaceTemperature(Temperature);
		if (Enabled) attached->EnableThermalRendering();
	}
	
	return attached;
}





