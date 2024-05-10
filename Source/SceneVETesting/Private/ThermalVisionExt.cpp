#include "ThermalVisionExt.h"

// Functions needed for SceneViewExtension
FThermalVisionExt::FThermalVisionExt(const FAutoRegister& AutoRegister) : FSceneViewExtensionBase(AutoRegister)
{
	UE_LOG(LogTemp, Log, TEXT("TestSceneViewExtension: Autoregister"));
	this->Enabled = false;
}

void FThermalVisionExt::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	UE_LOG(LogTemp, Log, TEXT("TestSceneViewExtension: Begin Render View Family"));
}

// This is called every frame, use to subscribe where needed - left the possible PassId's just for quick testing, you actually need to test what you need

void FThermalVisionExt::SubscribeToPostProcessingPass(EPostProcessingPass PassId, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
	if(!this->Enabled) return;

	if(!Noise) return;
	
	if (PassId == EPostProcessingPass::MotionBlur)
	{
//		UE_LOG(LogTemp, Warning, TEXT("TestSceneViewExtension: Pass is MotionBlur!"));
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FThermalVisionExt::ThermalVisionPass));
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

FScreenPassTexture FThermalVisionExt::ThermalVisionPass(FRDGBuilder& GraphBuilder, const FSceneView& SceneView, const FPostProcessMaterialInputs& InOutInputs)
{
	FCSInputParameters InputParameters;
	InputParameters.Noise = Noise;
	InputParameters.ColorStripe = ColorStripe;
	InputParameters.HeatResources = HeatSources;
	InputParameters.LowCut = 0.0f;
	InputParameters.TemperatureRange = 1.0f;
	InputParameters.HalfValueDepth = 512.0f;
	FScreenPassTexture SceneTexture = FSceneVEProcess::AddThermalProcessPass(GraphBuilder, SceneView, InOutInputs, InputParameters);
	return SceneTexture;
}

void FThermalVisionExt::UpdateHeatSources(const TArray<FHeatSourceMeta>& NewArray)
{
	HeatSources = NewArray;
}

TArray<FHeatSourceMeta> FThermalVisionExt::GetHeatResources()
{
	return HeatSources;
}
