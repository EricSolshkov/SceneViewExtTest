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
	if(TestSceneExtension)
	{
		this->TestSceneExtension->SetEnabled(Enabled);
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
	TestSceneExtension = FSceneViewExtensions::NewExtension<FThermalVisionExt>();
	UE_LOG(LogTemp, Log, TEXT("TestSceneViewExtension: Scene Extension Created!"));
}

void USceneVEComponent::UpdateHeatSources()
{
	// iterate through all actor, get all heat source components of every actor
	TActorIterator<AActor> ActorItr = TActorIterator<AActor>(GetWorld());
	TArray<UAcThermalManager*> ThmMgrs;
	HeatSources.Init(FHeatSourceMeta(), 0);
	for(; ActorItr; ++ActorItr)
	{
		TArray<UAcThermalManager*> Temp;
		
		ActorItr->GetComponents(Temp);
		ThmMgrs.Append(Temp);
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
	if (TestSceneExtension->IsEnabled())
	{
		UpdateHeatSources();
		TestSceneExtension->HeatSources = HeatSources;
		TestSceneExtension->Noise = Noise;
		TestSceneExtension->ColorStripe = ColorStripe;
	}
}
