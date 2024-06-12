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

	SetTemperatureHighCut(150);
	SetTemperatureLowCut(0);
}

// Called every frame
void UAcIntegratedSensor::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (SVExt->GetEnabledSensor())
	{
	case ThermalVision:
		SetColorStripe(ColorStripe);
		SetTemperatureLowCut(GetTemperatureLowCut());
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

void UAcIntegratedSensor::EnableNightVisionBoost()
{
	if(!SVExt.IsValid()) SVExt = ASVExtMgr::GetInstance(GetWorld())->GetExt();

	if(SVExt->GetEnabledSensor() == ESensorType::ThermalVision) DisableThermalVision();

	SVExt->SetEnabledSensor(ESensorType::NightVisionBoost);
	
}

void UAcIntegratedSensor::DisableNightVisionBoost()
{
	if(!SVExt.IsValid()) SVExt = ASVExtMgr::GetInstance(GetWorld())->GetExt();

	SVExt->SetEnabledSensor(ESensorType::None);
}

void UAcIntegratedSensor::EnableThermalVision()
{
	if (!SVExt.IsValid()) SVExt = ASVExtMgr::GetInstance(GetWorld())->GetExt();

	SetColorStripe(ColorStripe);

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
	
	SVExt->SetEnabledSensor(ESensorType::ThermalVision);
}

void UAcIntegratedSensor::DisableThermalVision()
{
	if(!SVExt.IsValid()) SVExt = ASVExtMgr::GetInstance(GetWorld())->GetExt();
	
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
	
	if(!SVExt) SVExt = ASVExtMgr::GetInstance(GetWorld())->GetExt();
	SVExt->ColorStripe = ColorStripe;
}

void UAcIntegratedSensor::SetTemperatureLowCut(float Low)
{
	if(!SVExt.IsValid()) SVExt = ASVExtMgr::GetInstance(GetWorld())->GetExt();
	
	SVExt->LowCut = Low;
	
	
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
	if(!SVExt.IsValid()) SVExt = ASVExtMgr::GetInstance(GetWorld())->GetExt();
	
	SVExt->HighCut = High;
	
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

