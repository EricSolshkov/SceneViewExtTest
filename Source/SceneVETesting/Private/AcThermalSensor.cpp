// Fill out your copyright notice in the Description page of Project Settings.


#include "AcThermalSensor.h"
#include "AcThermalManager.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Interfaces/Interface_PostProcessVolume.h"


// Sets default values for this component's properties
UAcThermalSensor::UAcThermalSensor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	IsThermalVisionEnabled = false;
}


// Called when the game starts
void UAcThermalSensor::BeginPlay()
{
	Super::BeginPlay();

	// ...
	World = GetWorld();

	if (!World)
	{
		return;
	}

	CameraManager = UGameplayStatics::GetPlayerCameraManager(World, 0);
	
}


// Called every frame
void UAcThermalSensor::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	
}

void UAcThermalSensor::EnableThermalVision()
{
	if (!World) return;
	
	for (TActorIterator<AActor> ActorIter(World); ActorIter; ++ActorIter)
	{
		UAcThermalManager* ThmMgr = (*ActorIter)->FindComponentByClass<UAcThermalManager>();
		if (ThmMgr)
		{
			ThmMgr->EnableThermalRendering();
		}
		else
		{
			UAcThermalManager::Create((*ActorIter), 30, true);
		}
	}
	IsThermalVisionEnabled = true;
}

void UAcThermalSensor::DisableThermalVision()
{
	if (!World) return;
	
	for (TActorIterator<AActor> ActorIter(World); ActorIter; ++ActorIter)
	{
		UAcThermalManager* ThmMgr = (*ActorIter)->FindComponentByClass<UAcThermalManager>();
		if (ThmMgr)
		{
			ThmMgr->DisableThermalRendering();
		}
	}
	IsThermalVisionEnabled = false;
}

bool UAcThermalSensor::GetThermalVisionEnabled()
{
	return IsThermalVisionEnabled;
}

void UAcThermalSensor::SetColorStripe(UTexture2D* Tex)
{
	if (MaterialInstance) MaterialInstance->SetTextureParameterValue(FName("ColorStripe"), Tex);
}

void UAcThermalSensor::SetTemperatureLowCut(float Low)
{
	if (MaterialInstance)
	{
		MaterialInstance->SetScalarParameterValue(FName("LowCut"), Low);
	}
}

void UAcThermalSensor::SetTemperatureHighCut(float High)
{
	if (MaterialInstance)
	{
		MaterialInstance->SetScalarParameterValue(FName("HighCut"), High);
	}
}

float UAcThermalSensor::GetTemperatureLowCut()
{
	float LowCut = 0;
	if (MaterialInstance)
	{
		MaterialInstance->GetScalarParameterValue(FName("LowCut"), LowCut);
	}
	return LowCut;
}

float UAcThermalSensor::GetTemperatureHighCut()
{
	float HighCut = 0;
	if (MaterialInstance)
	{
		MaterialInstance->GetScalarParameterValue(FName("HighCut"), HighCut);
	}
	return HighCut;
}

