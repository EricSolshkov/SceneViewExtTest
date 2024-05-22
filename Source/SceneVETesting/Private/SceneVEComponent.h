// Test project fo SceneViewExtension / RDG Shader Basic setup
// Copyright 2021 Ossi Luoto
// 
// SceneVEComponent - The SceneViewExtensionBase. Place this in the editor to an empty actor 

#pragma once

#include "CoreMinimal.h"
#include "HeatSource.h"
#include "Components/ActorComponent.h"
#include "Engine/VolumeTexture.h"

#include "IntegratedSVExt.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessMaterial.h"

#include "SceneVEComponent.generated.h"


// SceneVEComponent. Simple spawnable component to be place in the editor to an empty or other actor.
// This component is to construct/pass the input parameters of the shader for scene extension.
// Should be attached uniquely onto a "Camera-like" actor which is considered responsible for view generation.
// However, since this component get scene data(textures, buffers) directly from render pipeline,
// there is no data passing to attached actor and this component.
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SCENEVETESTING_API USceneVEComponent : public UActorComponent
{
	GENERATED_BODY()
	//Parameters
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UVolumeTexture* Noise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ColorStripe;
public:
	USceneVEComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void SetEnabled(const bool Enabled);
	
protected:
	virtual void BeginPlay() override;
	void CreateSceneViewExtension();

public:
	TArray<FHeatSourceMeta> HeatSources;

	UFUNCTION(CallInEditor, BlueprintCallable)
	void UpdateHeatSources();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// this is the way to store the SceneExtensionView to keep it safe from not being destroyed - from: SceneViewExtension.h
	TSharedPtr<class FIntegratedSVExt, ESPMode::ThreadSafe > TestSceneExtension;
};
