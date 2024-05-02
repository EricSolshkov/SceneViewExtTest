#include "TestSceneExtension.h"

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
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FTestSceneExtension::TestPostProcessPass_RT));
	}

	if (PassId == EPostProcessingPass::Tonemap)
	{
//		UE_LOG(LogTemp, Warning, TEXT("TestSceneViewExtension: Pass is Tonemap!"));
		
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
