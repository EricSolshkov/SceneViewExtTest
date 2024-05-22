#include "IntegratedSVExt.h"

#include "AcIntegratedSensor.h"
#include "AcThermalManager.h"

// Functions needed for SceneViewExtension
FIntegratedSVExt::FIntegratedSVExt(const FAutoRegister& AutoRegister) : FSceneViewExtensionBase(AutoRegister)
{
	UE_LOG(LogTemp, Log, TEXT("FIntegratedSVExt: Autoregister"));
	this->EnabledSensor = None;
}

void FIntegratedSVExt::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	//UE_LOG(LogTemp, Log, TEXT("FIntegratedSVExt: Begin Render View Family"));
}

// This is called every frame, use to subscribe where needed
//- left the possible PassId's just for quick testing, you actually need to test what you need
void FIntegratedSVExt::SubscribeToPostProcessingPass(EPostProcessingPass PassId, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
	switch(PassId)
	{
	case EPostProcessingPass::MotionBlur:
		AddSpecifiedExtenstion(InOutPassCallbacks);
		break;
	case EPostProcessingPass::Tonemap:
		break;
	case EPostProcessingPass::FXAA:
		break;
	case EPostProcessingPass::VisualizeDepthOfField:
		break;
	case EPostProcessingPass::MAX:
		break;
	}
}

void FIntegratedSVExt::PostRenderBasePass_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{
	FSceneViewExtensionBase::PostRenderBasePass_RenderThread(RHICmdList, InView);
}

// Extension comes with the Graphbuilder and SceneView plus set of PostProcessMaterialInputs - more on these at SceneViewExtension.h
// This is just an empty helper function now if we need to validate data or make other alterations
// I think otherwise we could directly subscribe the Process Function to the PassCallBack

FScreenPassTexture FIntegratedSVExt::ThermalVisionPass(FRDGBuilder& GraphBuilder, const FSceneView& SceneView, const FPostProcessMaterialInputs& InOutInputs)
{
	FThermalCSParams InputParameters;
	InputParameters.Noise = Noise;
	InputParameters.ColorStripe = ColorStripe;
	InputParameters.HeatSourceCount = HeatSources.Num();

	// Add a null Meta obj to avoid pass nullptr when creating buffer.
	if (InputParameters.HeatSourceCount == 0)
	{
		HeatSources.Add(FHeatSourceMeta());
	}
	
	InputParameters.HeatSources = HeatSources ;
	InputParameters.LowCut = LowCut;
	if(abs(HighCut - LowCut) <= 0.1f)
	{
		InputParameters.TemperatureRange = 0.1f;
	}
	else
	{
		InputParameters.TemperatureRange = HighCut - LowCut;
	}
	
	InputParameters.HalfValueDepth = HalfValueDepth;
	FScreenPassTexture SceneTexture = FSceneVEProcess::AddThermalProcessPass(
		GraphBuilder, SceneView, InOutInputs, InputParameters);
	return SceneTexture;
}

FScreenPassTexture FIntegratedSVExt::NightVisionBoostPass(FRDGBuilder& GraphBuilder, const FSceneView& SceneView,
	const FPostProcessMaterialInputs& InOutInputs)
{
	FNightVisionBoostPSParams InputParameters;
	InputParameters.BoostIntensity = 35;
	InputParameters.Jitter = 0.01;
	InputParameters.ColorStep = 0.00625;
	FScreenPassTexture SceneTexture = FSceneVEProcess::AddNightVisionBoostPass(
		GraphBuilder, SceneView, InOutInputs, InputParameters);
	return SceneTexture;
}

FScreenPassTexture FIntegratedSVExt::SyntheticAperturePass(FRDGBuilder& GraphBuilder, const FSceneView& SceneView,
	const FPostProcessMaterialInputs& InOutInputs)
{
	FSyntheticApertureParams InputParameters;
	FScreenPassTexture SceneTexture = FSceneVEProcess::AddSyntheticAperturePass(
		GraphBuilder, SceneView, InOutInputs, InputParameters);
	return SceneTexture;
}

void FIntegratedSVExt::AddSpecifiedExtenstion(FAfterPassCallbackDelegateArray& InOutPassCallbacks)
{
	switch(this->EnabledSensor)
	{
	case ThermalVision:
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FIntegratedSVExt::ThermalVisionPass));
		break;
	case NightVisionBoost:
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FIntegratedSVExt::NightVisionBoostPass));
		break;
	case SyntheticAperture:
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FIntegratedSVExt::SyntheticAperturePass));
		break;
	default:
		break;
	}
}
