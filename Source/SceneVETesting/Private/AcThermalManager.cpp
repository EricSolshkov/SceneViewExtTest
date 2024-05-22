// Fill out your copyright notice in the Description page of Project Settings.


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

void UAcThermalManager::SetTemperatureHighCut(const float NewTemperature)
{
	if(ThermalMaterialInstance)
	{
		MeshComponent->SetScalarParameterValueOnMaterials(FName("HighCut"), NewTemperature);
	}
}

float UAcThermalManager::GetTemperatureHighCut()
{
	if(ThermalMaterialInstance)
	{
		return MeshComponent->GetScalarParameterDefaultValue(FName("HighCut"));
	}
	return 0;
}

void UAcThermalManager::SetTemperatureLowCut(const float NewTemperature)
{
	if(ThermalMaterialInstance)
	{
		MeshComponent->SetScalarParameterValueOnMaterials(FName("LowCut"), NewTemperature);
	}
}

float UAcThermalManager::GetTemperatureLowCut()
{
	if(ThermalMaterialInstance)
	{
		return MeshComponent->GetScalarParameterDefaultValue(FName("LowCut"));
	}
	return 0;
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
			Container.Add(Hr->GetMeta());
	}
}

UAcThermalManager* UAcThermalManager::Create(AActor* Actor, float Temperature, bool Enabled)
{
	// Check to avoid creating redundant ThermalManager for invisible actors.
	auto mesh = Actor->FindComponentByClass<UMeshComponent>();
	if (!mesh) return nullptr;

	// Check to avoid creating ThermalManager for billboard and text renderers.
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





