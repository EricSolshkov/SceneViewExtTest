// Test project fo SceneViewExtension / RDG Shader Basic setup
// Copyright 2021 Ossi Luoto
// 
// SceneVEComponent - The SceneViewExtensionBase. Place this in the editor to an empty actor 

#include "SceneVEComponent.h"

#include <Actor.h>

#include "AcThermalManager.h"
#include "SceneVEProcess.h"
#include "GameFramework/Actor.h"
#include "HeatSource.h"

#include "EngineUtils.h"
#include "BehaviorTree/BehaviorTreeTypes.h"



USceneVEComponent::USceneVEComponent(const FObjectInitializer& ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USceneVEComponent::SetEnabled(const bool Enabled)
{
	if(!TestSceneExtension) return;
	if(Enabled)
	{
		this->TestSceneExtension->SetEnabledSensor(ThermalVision);
	}
	else
	{
		TestSceneExtension->SetEnabledSensor(None);
	}
}

// Called when the game starts
void USceneVEComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("TestSceneViewExtension: Component BeginPlay!"));
	if (!TestSceneExtension)
	{
		CreateSceneViewExtension();
	}
	UpdateHeatSources();
	TestSceneExtension->HeatSources = HeatSources;
	TestSceneExtension->Noise = Noise;
	TestSceneExtension->ColorStripe = ColorStripe;
}


// On a separate function to hook f.ex. for in editor creation etc.
void USceneVEComponent::CreateSceneViewExtension()
{
	TestSceneExtension = FSceneViewExtensions::NewExtension<FIntegratedSVExt>();
	UE_LOG(LogTemp, Log, TEXT("TestSceneViewExtension: Scene Extension Created!"));
}

void USceneVEComponent::UpdateHeatSources()
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

// Called every frame
void USceneVEComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TestSceneExtension->GetEnabledSensor())
	{
		UpdateHeatSources();
		TestSceneExtension->HeatSources = HeatSources;
		TestSceneExtension->Noise = Noise;
		TestSceneExtension->ColorStripe = ColorStripe;
	}
}
