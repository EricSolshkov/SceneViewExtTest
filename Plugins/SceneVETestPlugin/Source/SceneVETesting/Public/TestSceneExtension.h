#pragma once
#include "CoreMinimal.h"
#include "SceneViewExtension.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessMaterial.h"

#include "HeatResource.h"
#include "SceneVEProcess.h"

// Use FSceneViewExtensionBase to extend hook properly
class FTestSceneExtension : public FSceneViewExtensionBase
{
public:
	FTestSceneExtension(const FAutoRegister& AutoRegister);

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
	FScreenPassTexture TestPostProcessPass_RT(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& InOutInputs);
	
	
private:
	bool Enabled;

public:
	UVolumeTexture* Noise = nullptr;
	
	TArray<FHeatResource> HeatResources;

	void UpdateHeatResourceArray(const TArray<FHeatResource>& NewArray);

	TArray<FHeatResource> GetHeatResources();
};

