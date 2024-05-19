// Fill out your copyright notice in the Description page of Project Settings.


#include "AcThermalSensor.h"
#include "AcThermalManager.h"

#include "Camera/CameraActor.h"


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

	if(!GetWorld()) return;

	CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	CreateSceneViewExtension();
	
}

// On a separate function to hook f.ex. for in editor creation etc.
void UAcThermalSensor::CreateSceneViewExtension()
{
	ThermalSVExtension = FSceneViewExtensions::NewExtension<FThermalVisionExt>();
	UE_LOG(LogTemp, Log, TEXT("UAcThermalSensor: Scene Extension Created!"));
}


// Called every frame
void UAcThermalSensor::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ThermalSVExtension->IsEnabled())
	{
		UpdateHeatSources();
		ThermalSVExtension->HeatSources = HeatSources;
		ThermalSVExtension->Noise = Noise;
		ThermalSVExtension->ColorStripe = ColorStripe;
	}
	
}

void UAcThermalSensor::UpdateHeatSources()
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
}

void UAcThermalSensor::EnableThermalVision()
{
	if (ThermalSVExtension)
	{
		ThermalSVExtension->SetEnabled(true);
	}
	
	for (TActorIterator<AActor> ActorIter(GetWorld()); ActorIter; ++ActorIter)
	{
		UAcThermalManager* ThmMgr = (*ActorIter)->FindComponentByClass<UAcThermalManager>();

		// This assignment does NOT guarantee ThmMge non-null.
		// Because Create() returns null if *ActorIter contains no mesh comps(invisible).
		if (ThmMgr == nullptr)
		{
			ThmMgr = UAcThermalManager::Create((*ActorIter), 30, true);
		}

		if(ThmMgr != nullptr)
		{
			auto list1 = ThmMgr->GetOwner()->FindComponentByClass<UStaticMeshComponent>()->GetMaterials();
			ThmMgr->EnableThermalRendering();
			list1 = ThmMgr->GetOwner()->FindComponentByClass<UStaticMeshComponent>()->GetMaterials();
			
		}
	}
	IsThermalVisionEnabled = true;
}

void UAcThermalSensor::DisableThermalVision()
{
	if (ThermalSVExtension)
	{
		ThermalSVExtension->SetEnabled(false);
	}
	
	for (TActorIterator<AActor> ActorIter(GetWorld()); ActorIter; ++ActorIter)
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
	ColorStripe = Tex;
}

void UAcThermalSensor::SetTemperatureLowCut(float Low)
{
	if (ThermalSVExtension)
	{
		ThermalSVExtension->LowCut = Low;
	}
	
	for (TActorIterator<AActor> ActorIter(GetWorld()); ActorIter; ++ActorIter)
	{
		UAcThermalManager* ThmMgr = (*ActorIter)->FindComponentByClass<UAcThermalManager>();
		if (ThmMgr)
		{
			ThmMgr->SetLowCutTemperature(Low);
		}
	}
}

void UAcThermalSensor::SetTemperatureHighCut(float High)
{
	if (ThermalSVExtension)
	{
		ThermalSVExtension->HighCut = High;
	}
	
	for (TActorIterator<AActor> ActorIter(GetWorld()); ActorIter; ++ActorIter)
	{
		UAcThermalManager* ThmMgr = (*ActorIter)->FindComponentByClass<UAcThermalManager>();
		if (ThmMgr)
		{
			ThmMgr->SetHighCutTemperature(High);
		}
	}
}

float UAcThermalSensor::GetTemperatureLowCut()
{
	if (ThermalSVExtension)
	{
		return ThermalSVExtension->LowCut;
	}
	return 0;
}

float UAcThermalSensor::GetTemperatureHighCut()
{
	if (ThermalSVExtension)
	{
		return ThermalSVExtension->HighCut;
	}
	return 0;
}

void UAcThermalSensor::SetHalfValueDepth(float Depth)
{
	if (ThermalSVExtension)
	{
		ThermalSVExtension->HalfValueDepth = Depth;
	}
}

