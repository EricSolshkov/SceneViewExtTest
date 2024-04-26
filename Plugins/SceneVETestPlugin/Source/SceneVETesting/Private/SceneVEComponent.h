// Test project fo SceneViewExtension / RDG Shader Basic setup
// Copyright 2021 Ossi Luoto
// 
// SceneVEComponent - The SceneViewExtensionBase. Place this in the editor to an empty actor 

#pragma once

#include "CoreMinimal.h"
#include "HeatResource.h"
#include "Components/ActorComponent.h"
#include "Engine/VolumeTexture.h"

#include "TestSceneExtension.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessMaterial.h"

#include "SceneVEComponent.generated.h"


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
	void SetEnabled(const bool Enabled);

	UFUNCTION(CallInEditor)
	void EnablePreview() { SetEnabled(true); }

	UFUNCTION(CallInEditor)
	void DisablePreview() { SetEnabled(false); }
	
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
