// Test project fo SceneViewExtension / RDG Shader Basic setup
// Copyright 2021 Ossi Luoto
// 
// SceneVEComponent - The SceneViewExtensionBase. Place this in the editor to an empty actor 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/VolumeTexture.h"
#include "RenderGraphUtils.h"

#include "SceneViewExtension.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessMaterial.h"

#include "SceneVEComponent.generated.h"

USTRUCT(Blueprintable)
struct FHeatResource
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Center;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Color;
	
public:
	FHeatResource()
	{
		Center = FVector(0,0,0);
		Radius = 0;
		Color = FVector(0,0,0);
	};
	
	FHeatResource(const FVector& iCenter, const FVector& iColor, float iRadius)
	{
		Center = iCenter;
		Radius = iRadius;
		Color = iColor;
	}
	
};

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

// SceneVEComponent. Simple spawnable component to be place in the editor to an empty or other actor

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SCENEVETESTING_API USceneVEComponent : public UActorComponent
{
	GENERATED_BODY()
	//Parameters
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UVolumeTexture* Noise;
public:
	USceneVEComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void SetEnabled(const bool Enabled) { if(TestSceneExtension) this->TestSceneExtension->SetEnabled(Enabled); }
	
protected:
	virtual void BeginPlay() override;
	void CreateSceneViewExtension();

public:
	TArray<FHeatResource> HeatResources;
	TArray<FHeatResource> InitParameterArray();
	void UpdateHeatResources();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// this is the way to store the SceneExtensionView to keep it safe from not being destroyed - from: SceneViewExtension.h
	TSharedPtr<class FTestSceneExtension, ESPMode::ThreadSafe > TestSceneExtension;
};
