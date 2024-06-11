#pragma once
#include "CoreMinimal.h"
#include "SceneViewExtension.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessMaterial.h"

#include "HeatSource.h"
#include "SceneVEProcess.h"

UENUM()
enum ESensorType
{
	None				UMETA(DisplayName="None"),
	ThermalVision		UMETA(DisplayName="ThermalVision"),
	NightVisionBoost	UMETA(DisplayName="NightVisionBoost"),
	SyntheticAperture	UMETA(DisplayName="SyntheticAperture(NOT IMPLEMENTED!)")   // NOT IMPLEMENTED!
};

// This Extension implement handles interaction with Unreal render pipeline(Renderer::Render())
// This implement CARES exact sensor type.
class FIntegratedSVExt : public FSceneViewExtensionBase  // Use FSceneViewExtensionBase to extend hook properly
{
public:
	FIntegratedSVExt(const FAutoRegister& AutoRegister);

	// These must all be set
public:
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {};
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {};
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override; // like component tick
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override {};
	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override {};
	virtual void SubscribeToPostProcessingPass(EPostProcessingPass PassId, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled) override;
	virtual void PostRenderBasePass_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override;

	void SetEnabledSensor(const ESensorType SensorType)
	{
		this->EnabledSensor = SensorType;
		
#if WITH_EDITOR
		const UEnum* EObj = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESensorType"));
		UE_LOG(LogTemp, Log, TEXT("[IntegratedSVExt] Set sensor to %s"), *(EObj->GetNameByValue(this->EnabledSensor).ToString()));
#endif
	}

	ESensorType GetEnabledSensor() const
	{
		return this->EnabledSensor;
	}
	
	// This is our actual hook function
	FScreenPassTexture ThermalVisionPass(FRDGBuilder& GraphBuilder, const FSceneView& SceneView, const FPostProcessMaterialInputs& InOutInputs);
	FScreenPassTexture NightVisionBoostPass(FRDGBuilder& GraphBuilder, const FSceneView& SceneView, const FPostProcessMaterialInputs& InOutInputs);
	FScreenPassTexture SyntheticAperturePass(FRDGBuilder& GraphBuilder, const FSceneView& SceneView, const FPostProcessMaterialInputs& InOutInputs);
	
private:
	ESensorType EnabledSensor;

public:
	UVolumeTexture* VolumeNoise = nullptr;

	UTexture2D* Noise2D = nullptr;
	
	UTexture2D* ColorStripe = nullptr;
	
	TArray<AHeatSourceBase*> HeatSources;

	float HighCut = 150.0f;
	
	float LowCut = 0.0f;

	float HalfValueDepth = 512.0f;

	float GameTime = 0.0f;

	void AddSpecifiedExtenstion(FAfterPassCallbackDelegateArray& InOutPassCallbacks);
};

