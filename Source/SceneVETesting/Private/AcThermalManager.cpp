// Fill out your copyright notice in the Description page of Project Settings.


#include "AcThermalManager.h"
#include "HeatSource.h"
#include "ThermalMaterialPtr.h"

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

	TemperatureHighCut = 150;

	TemperatureLowCut = 0;

	HeatSources.Empty();
}

void SaveOriginalMaterials(AActor* MyActor, UAcThermalManager* ATM)
{
	const auto ParentMaterial = FThermalMaterialPtr::Get();
	check(ParentMaterial);
	
	ATM->MeshComponent = MyActor->FindComponentByClass<UMeshComponent>();
	if (ATM->MeshComponent)
	{
		ATM->OriginalMaterials = ATM->MeshComponent->GetMaterials();
		ATM->ThermalMaterialInstances.Empty();
		ATM->OriginalTexes.Empty();
		TArray<UTexture*> TexContainer = {};
		for (const auto& M : ATM->OriginalMaterials)
		{
			TexContainer.Empty();
			M->GetUsedTextures(
				TexContainer,
				EMaterialQualityLevel::Low,
				false,
				ERHIFeatureLevel::SM5,
				false
				);
			if(TexContainer.Num() != 0) {
				UTexture2D* T2D = Cast<UTexture2D>(TexContainer[0]);
				ATM->OriginalTexes.Add(T2D);
			}
			else
			{
				ATM->OriginalTexes.Add((UTexture2D*)nullptr);
			}
			auto ThermalMaterialInstance = UMaterialInstanceDynamic::Create(ParentMaterial, ATM);
			check(ThermalMaterialInstance);
			ATM->ThermalMaterialInstances.Add(ThermalMaterialInstance);
		}
	}
	
}

// Called when the game starts
void UAcThermalManager::BeginPlay()
{
	Super::BeginPlay();
	
	// Store original mesh material.
	const auto MyActor = GetOwner();
	check(MyActor);
	SaveOriginalMaterials(MyActor, this);
	
	SetTemperatureHighCut(TemperatureHighCut);
	SetTemperatureLowCut(TemperatureLowCut);
}


// Called every frame
void UAcThermalManager::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update Thermal Material Temperature Parameter.
	if (ThermalRenderingEnabled)
	{
		for (auto i = 0; i < ThermalMaterialInstances.Num(); ++i)
		{
			ThermalMaterialInstances[i]->SetScalarParameterValue(FName("SurfaceTemperature"), SurfaceTemperature);
			if(IsValid(OriginalTexes[i]))
				ThermalMaterialInstances[i]->SetTextureParameterValue(FName("OriginalTex"), OriginalTexes[i]);
		}
		
		SetTemperatureHighCut(TemperatureHighCut);
		SetTemperatureLowCut(TemperatureLowCut);
	}
}

void UAcThermalManager::SetTemperatureHighCut(const float NewTemperature)
{
	TemperatureHighCut = NewTemperature;
	if (GetThermalRenderingStatus())
		MeshComponent->SetScalarParameterValueOnMaterials(FName("HighCut"), NewTemperature);
}

float UAcThermalManager::GetTemperatureHighCut()
{
	return TemperatureHighCut;
}

void UAcThermalManager::SetTemperatureLowCut(const float NewTemperature)
{
	TemperatureLowCut = NewTemperature;
	if (GetThermalRenderingStatus())
		MeshComponent->SetScalarParameterValueOnMaterials(FName("LowCut"), NewTemperature);
}

float UAcThermalManager::GetTemperatureLowCut()
{
	return TemperatureLowCut;
}

void UAcThermalManager::EnableThermalRendering()
{
	if (!ThermalRenderingEnabled)
	{
		ThermalRenderingEnabled = true;
		
		for (int i = 0; i < OriginalMaterials.Num(); ++i)
		{
			MeshComponent->SetMaterial(i, ThermalMaterialInstances[i]);
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

void UAcThermalManager::AppendHeatSources(TArray<AHeatSourceBase*>& Container)
{
	for(auto Hr : HeatSources)
	{
		if(IsValid(Hr))
			Container.Add(Hr);
	}
}

UAcThermalManager* UAcThermalManager::Create(AActor* Actor, float Temperature, bool Enabled)
{
	// Check to avoid creating redundant ThermalManager for invisible actors.
	const auto mesh = Actor->FindComponentByClass<UMeshComponent>();
	if (!mesh) return nullptr;

	// Avoid Create ThrMgr for HeatSources
	const auto Hr = Cast<AHeatSourceBase, AActor>(Actor);
	if (IsValid(Hr)) return nullptr;

	// Check to avoid creating ThermalManager for billboard and text renderers.
	// Due to ThermalVisionCS is working on each pixel and lack stencil check, billboard and text pixels will be mapped.
	// this avoidance will NOT achieve its goal.
	const auto Bill = Cast<UBillboardComponent, UMeshComponent>(mesh);
	if (Bill) return nullptr;
	const auto Text = Cast<UTextRenderComponent, UMeshComponent>(mesh);
	if (Text) return nullptr;
	

    // Check to avoid creating duplicate ThermalManager
	const auto Atm = Actor->FindComponentByClass<UAcThermalManager>();
	if (Atm) return Atm;

	const FTransform Transform = UKismetMathLibrary::MakeTransform(
		FVector(0.0, 0.0, 0.0),
		FRotator(0.0, 0.0, 0.0),
		FVector(1.0, 1.0, 1.0));
	const auto Component = Actor->AddComponentByClass(StaticClass(), false, Transform, false);
	const auto ThermalManager = Cast<UAcThermalManager, UActorComponent>(Component);

	// Here we require and assume TargetThermalManagerComponent is not nullptr.
	// if this assume fails program crashes here, check then.
	const auto ParentMaterial = FThermalMaterialPtr::Get();
	check(ParentMaterial);
	
	SaveOriginalMaterials(Actor, ThermalManager);
	ThermalManager->SetSurfaceTemperature(Temperature);
	if (Enabled) ThermalManager->EnableThermalRendering();
	
	return ThermalManager;
}

void UAcThermalManager::AddSphere()
{
	// CreateDefaultSubObject works only in constructors.
	// CreateNewObj can be called runtime.
	const auto Hr = Cast<ASphereHeatSource, AActor>(GetWorld()->SpawnActor(ASphereHeatSource::StaticClass()));
	HeatSources.Add(Hr);
	Hr->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
}

void UAcThermalManager::AddBox()
{
	// CreateDefaultSubObject works only in constructors.
	// CreateNewObj can be called runtime.
	const auto Hr = Cast<ABoxHeatSource, AActor>(GetWorld()->SpawnActor(ABoxHeatSource::StaticClass()));
	HeatSources.Add(Hr);
	Hr->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
}

void UAcThermalManager::AddCapsule()
{
	// CreateDefaultSubObject works only in constructors.
	// CreateNewObj can be called runtime.
	const auto Hr = Cast<ACapsuleHeatSource, AActor>(GetWorld()->SpawnActor(ACapsuleHeatSource::StaticClass()));
	HeatSources.Add(Hr);
	Hr->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
}





