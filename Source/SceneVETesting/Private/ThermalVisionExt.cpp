#include "ThermalVisionExt.h"

#include "AcThermalManager.h"

// Functions needed for SceneViewExtension
FThermalVisionExt::FThermalVisionExt(const FAutoRegister& AutoRegister) : FSceneViewExtensionBase(AutoRegister)
{
	UE_LOG(LogTemp, Log, TEXT("FThermalVisionExt: Autoregister"));
	this->Enabled = false;
}

void FThermalVisionExt::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	UE_LOG(LogTemp, Log, TEXT("FThermalVisionExt: Begin Render View Family"));
}

// This is called every frame, use to subscribe where needed - left the possible PassId's just for quick testing, you actually need to test what you need

void FThermalVisionExt::SubscribeToPostProcessingPass(EPostProcessingPass PassId, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
	if(!this->Enabled) return;

	if(!Noise)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExtensionSubscribe Failed: No valid noise texture."));
		return;
	};
	
	if (PassId == EPostProcessingPass::MotionBlur)
	{
//		UE_LOG(LogTemp, Warning, TEXT("FThermalVisionExt: Pass is MotionBlur!"));
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FThermalVisionExt::ThermalVisionPass));
	}

	if (PassId == EPostProcessingPass::Tonemap)
	{
//		UE_LOG(LogTemp, Warning, TEXT("FThermalVisionExt: Pass is Tonemap!"));
		
	}

	if (PassId == EPostProcessingPass::FXAA)
	{
//		UE_LOG(LogTemp, Warning, TEXT("FThermalVisionExt: Pass is FXAA!"));
	}

	if (PassId == EPostProcessingPass::VisualizeDepthOfField)
	{
//		UE_LOG(LogTemp, Warning, TEXT("FThermalVisionExt: Pass is VisualizeDepthOfField!"));
	}

	if (PassId == EPostProcessingPass::MAX)
	{
//		UE_LOG(LogTemp, Warning, TEXT("FThermalVisionExt: Pass is MAX!"));
	}
}

void FThermalVisionExt::PostRenderBasePass_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{
	FSceneViewExtensionBase::PostRenderBasePass_RenderThread(RHICmdList, InView);
	
}

// Extension comes with the Graphbuilder and SceneView plus set of PostProcessMaterialInputs - more on these at SceneViewExtension.h
// This is just an empty helper function now if we need to validate data or make other alterations
// I think otherwise we could directly subscribe the Process Function to the PassCallBack

FScreenPassTexture FThermalVisionExt::ThermalVisionPass(FRDGBuilder& GraphBuilder, const FSceneView& SceneView, const FPostProcessMaterialInputs& InOutInputs)
{
	FCSInputParameters InputParameters;
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
