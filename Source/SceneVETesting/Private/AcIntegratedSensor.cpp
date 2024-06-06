// Fill out your copyright notice in the Description page of Project Settings.


#include "AcIntegratedSensor.h"
#include "AcThermalManager.h"

#include "Camera/CameraActor.h"


// Sets default values for this component's properties
UAcIntegratedSensor::UAcIntegratedSensor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAcIntegratedSensor::BeginPlay()
{
	Super::BeginPlay();

	if(!GetWorld()) return;

	CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	VolumeNoise = LoadObject<UVolumeTexture>(nullptr, TEXT("VolumeTexture'/SceneVETestPlugin/VT_PerlinWorley_Balanced.VT_PerlinWorley_Balanced'"));
	check(VolumeNoise);

	Noise2D = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Engine/EngineMaterials/Good64x64TilingNoiseHighFreq.Good64x64TilingNoiseHighFreq'"));
	check(Noise2D);
	
	CreateSceneViewExtension();
	
}

// On a separate function to hook f.ex. for in editor creation etc.
void UAcIntegratedSensor::CreateSceneViewExtension()
{
	SVExt = FSceneViewExtensions::NewExtension<FIntegratedSVExt>();
	SVExt->VolumeNoise = VolumeNoise;
	SVExt->Noise2D = Noise2D;
	UE_LOG(LogTemp, Log, TEXT("UAcIntegratedSensor: Scene Extension Created!"));
}


// Called every frame
void UAcIntegratedSensor::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (SVExt->GetEnabledSensor())
	{
	case ThermalVision:
		UpdateHeatSources();
		
		SVExt->ColorStripe = ColorStripe;
		break;
	case NightVisionBoost:
		SVExt->GameTime = GetWorld()->GetTimeSeconds();
		break;
	case SyntheticAperture:
		break;
	default:
		break;
	}
	
}

void UAcIntegratedSensor::UpdateHeatSources()
{
	HeatSources.Empty();
	TArray<UAcThermalManager*> ThmMgrs;
	// iterate through all actor, get all heat source components of every actor
	for(TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		TArray<UAcThermalManager*> Comps;
		(*ActorItr)->GetComponents(Comps);
		if(Comps.Num() > 0)
		{
			ThmMgrs.Append(Comps);
		}
	}
	for (auto ThmMgr : ThmMgrs)
	{
		ThmMgr->AppendHeatSourcesMeta(HeatSources);
	}
	SVExt->HeatSources = HeatSources;
}

void UAcIntegratedSensor::EnableNightVisionBoost()
{
	if(!SVExt.IsValid()) return;

	if(SVExt->GetEnabledSensor() == ESensorType::ThermalVision) DisableThermalVision();

	SVExt->SetEnabledSensor(ESensorType::NightVisionBoost);
	
}

void UAcIntegratedSensor::DisableNightVisionBoost()
{
	if(!SVExt.IsValid()) return;

	SVExt->SetEnabledSensor(ESensorType::None);
}

void UAcIntegratedSensor::EnableThermalVision()
{
	if (!SVExt.IsValid()) return;
	
	SVExt->SetEnabledSensor(ESensorType::ThermalVision);
	
	for (TActorIterator<AActor> ActorIter(GetWorld()); ActorIter; ++ActorIter)
	{
		UAcThermalManager* ThmMgr = (*ActorIter)->FindComponentByClass<UAcThermalManager>();

		// This assignment does NOT guarantee ThmMgr non-null.
		// Because Create() returns null if *ActorIter contains no mesh comps(invisible).
		if (ThmMgr == nullptr)
		{
			ThmMgr = UAcThermalManager::Create((*ActorIter), 30, true);
		}

		if(ThmMgr != nullptr)
		{
			ThmMgr->EnableThermalRendering();
		}
	}
}

void UAcIntegratedSensor::DisableThermalVision()
{
	if (!SVExt.IsValid()) return;
	
	SVExt->SetEnabledSensor(ESensorType::None);
	
	for (TActorIterator<AActor> ActorIter(GetWorld()); ActorIter; ++ActorIter)
	{
		UAcThermalManager* ThmMgr = (*ActorIter)->FindComponentByClass<UAcThermalManager>();
		if (ThmMgr)
		{
			ThmMgr->DisableThermalRendering();
		}
	}
}

void UAcIntegratedSensor::SetColorStripe(UTexture2D* Tex)
{
	ColorStripe = Tex;
}

void UAcIntegratedSensor::SetTemperatureLowCut(float Low)
{
	if (SVExt)
	{
		SVExt->LowCut = Low;
	}
	
	for (TActorIterator<AActor> ActorIter(GetWorld()); ActorIter; ++ActorIter)
	{
		UAcThermalManager* ThmMgr = (*ActorIter)->FindComponentByClass<UAcThermalManager>();
		if (ThmMgr)
		{
			ThmMgr->SetTemperatureLowCut(Low);
		}
	}
}

void UAcIntegratedSensor::SetTemperatureHighCut(float High)
{
	if (SVExt)
	{
		SVExt->HighCut = High;
	}
	
	for (TActorIterator<AActor> ActorIter(GetWorld()); ActorIter; ++ActorIter)
	{
		UAcThermalManager* ThmMgr = (*ActorIter)->FindComponentByClass<UAcThermalManager>();
		if (ThmMgr)
		{
			ThmMgr->SetTemperatureHighCut(High);
		}
	}
}

float UAcIntegratedSensor::GetTemperatureLowCut()
{
	if (SVExt)
	{
		return SVExt->LowCut;
	}
	return 0;
}

float UAcIntegratedSensor::GetTemperatureHighCut()
{
	if (SVExt)
	{
		return SVExt->HighCut;
	}
	return 0;
}

void UAcIntegratedSensor::SetHalfValueDepth(float Depth)
{
	if (SVExt)
	{
		SVExt->HalfValueDepth = Depth;
	}
}

