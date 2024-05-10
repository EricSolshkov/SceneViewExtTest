#pragma once
#include "CoreMinimal.h"
#include "SceneViewExtension.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessMaterial.h"

#include "HeatSource.h"
#include "SceneVEProcess.h"

// Use FSceneViewExtensionBase to extend hook properly
class FThermalVisionExt : public FSceneViewExtensionBase
{
public:
	FThermalVisionExt(const FAutoRegister& AutoRegister);

	// These must all be set
public:
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {};
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {};
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override; // like component tick
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override {};
	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override {};
	virtual void SubscribeToPostProcessingPass(EPostProcessingPass PassId, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled);

	void SetEnabled(const bool NewEnabled) { this->Enabled = NewEnabled; UE_LOG(LogTemp, Log, TEXT("set to %sabled"), this->Enabled?"en":"dis");}
	bool IsEnabled() { return this->Enabled;}
	// This is our actual hook function
	FScreenPassTexture ThermalVisionPass(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& InOutInputs);
	
	
private:
	bool Enabled;

public:
	UVolumeTexture* Noise = nullptr;

	UTexture2D* ColorStripe = nullptr;
	
	TArray<FHeatSourceMeta> HeatSources;

	void UpdateHeatSources(const TArray<FHeatSourceMeta>& NewArray);

	TArray<FHeatSourceMeta> GetHeatResources();
};

