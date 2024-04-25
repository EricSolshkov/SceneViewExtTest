// Test project fo SceneViewExtension / RDG Shader Basic setup
// Copyright 2021 Ossi Luoto
// 
// SceneVEComponent - The SceneViewExtensionBase. Place this in the editor to an empty actor 

#include "SceneVEComponent.h"
#include "SceneVEProcess.h"
#include "BehaviorTree/BehaviorTreeTypes.h"

// Functions needed for SceneViewExtension
FTestSceneExtension::FTestSceneExtension(const FAutoRegister& AutoRegister) : FSceneViewExtensionBase(AutoRegister)
{
	UE_LOG(LogTemp, Log, TEXT("TestSceneViewExtension: Autoregister"));
	this->Enabled = false;
}

void FTestSceneExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	UE_LOG(LogTemp, Log, TEXT("TestSceneViewExtension: Begin Render View Family"));
}

// This is called every frame, use to subscribe where needed - left the possible PassId's just for quick testing, you actually need to test what you need

void FTestSceneExtension::SubscribeToPostProcessingPass(EPostProcessingPass PassId, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
	if(!this->Enabled) return;

	if(!Noise) return;
	
	if (PassId == EPostProcessingPass::MotionBlur)
	{
//		UE_LOG(LogTemp, Warning, TEXT("TestSceneViewExtension: Pass is MotionBlur!"));
		
	}

	if (PassId == EPostProcessingPass::Tonemap)
	{
//		UE_LOG(LogTemp, Warning, TEXT("TestSceneViewExtension: Pass is Tonemap!"));
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FTestSceneExtension::TestPostProcessPass_RT));
	}

	if (PassId == EPostProcessingPass::FXAA)
	{
//		UE_LOG(LogTemp, Warning, TEXT("TestSceneViewExtension: Pass is FXAA!"));
	}

	if (PassId == EPostProcessingPass::VisualizeDepthOfField)
	{
//		UE_LOG(LogTemp, Warning, TEXT("TestSceneViewExtension: Pass is VisualizeDepthOfField!"));
	}

	if (PassId == EPostProcessingPass::MAX)
	{
//		UE_LOG(LogTemp, Warning, TEXT("TestSceneViewExtension: Pass is MAX!"));
	}
}

// Extension comes with the Graphbuilder and SceneView plus set of PostProcessMaterialInputs - more on these at SceneViewExtension.h
// This is just an empty helper function now if we need to validate data or make other alterations
// I think otherwise we could directly subscribe the Process Function to the PassCallBack

FScreenPassTexture FTestSceneExtension::TestPostProcessPass_RT(FRDGBuilder& GraphBuilder, const FSceneView& SceneView, const FPostProcessMaterialInputs& InOutInputs)
{
	FScreenPassTexture SceneTexture = FSceneVEProcess::AddSceneVETestPass(GraphBuilder, SceneView, InOutInputs, HeatResources, Noise);
	return SceneTexture;
}

void FTestSceneExtension::UpdateHeatResourceArray(const TArray<FHeatResource>& NewArray)
{
	HeatResources = NewArray;
}

TArray<FHeatResource> FTestSceneExtension::GetHeatResources()
{
	return HeatResources;
}

USceneVEComponent::USceneVEComponent(const FObjectInitializer& ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void USceneVEComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("TestSceneViewExtension: Component BeginPlay!"));
	CreateSceneViewExtension();
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
