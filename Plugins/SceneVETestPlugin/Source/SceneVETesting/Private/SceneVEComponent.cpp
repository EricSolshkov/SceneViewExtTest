// Test project fo SceneViewExtension / RDG Shader Basic setup
// Copyright 2021 Ossi Luoto
// 
// SceneVEComponent - The SceneViewExtensionBase. Place this in the editor to an empty actor 

#include "SceneVEComponent.h"
#include "SceneVEProcess.h"
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
	HeatResources = InitParameterArray();
	TestSceneExtension->HeatResources = HeatResources;
	TestSceneExtension->Noise = Noise;
}


// On a separate function to hook f.ex. for in editor creation etc.
void USceneVEComponent::CreateSceneViewExtension()
{
	TestSceneExtension = FSceneViewExtensions::NewExtension<FTestSceneExtension>();
	UE_LOG(LogTemp, Log, TEXT("TestSceneViewExtension: Scene Extension Created!"));
}

TArray<FHeatResource> USceneVEComponent::InitParameterArray()
{
	int Count = 512;
	// Temperary Array Initializing;
	FHeatResource Hr = FHeatResource(FVector::ZeroVector, FVector::ZeroVector, 256.0f);
	HeatResources.Init(Hr, Count);
	for (auto& hr : HeatResources)
	{
		hr.Center = FVector(
			FMath::RandRange(-512.0f, 512.0f),
			FMath::RandRange(-512.0f, 512.0f),
			FMath::RandRange(-50.0f, 50.0f));
		hr.Radius = FMath::RandRange(8.0f, 64.0f);
		hr.Color = FVector(
			FMath::RandRange(0.0f, 1.0f),
			FMath::RandRange(0.0f, 1.0f),
			FMath::RandRange(0.0f, 1.0f));
	}
	return HeatResources;
}

void USceneVEComponent::UpdateHeatResources()
{
	for(FHeatResource& hr : HeatResources)
	{
		hr.Center += FVector(
			FMath::RandRange(-0.5f, 0.5f),
			FMath::RandRange(-0.5f, 0.5f),
			0
		);
	}
	TestSceneExtension->HeatResources = HeatResources;
}


// Called every frame
void USceneVEComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TestSceneExtension->IsEnabled()) UpdateHeatResources();
}
