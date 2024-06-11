// Test project fo SceneExtensionView / RDG Shader Basic setup
// Copyright 2021 Ossi Luoto
// 
// SceneVEProcess.cpp - Actual RDG hook and Shader Loading

#pragma once

#include "ScreenPass.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessMaterial.h"
#include "Engine/VolumeTexture.h"

#include "..\Public\HeatSource.h"

struct FThermalCSParams
{
	TArray<FSphereMeta> SphereHeatSources;
	int SphereCount;
	TArray<FBoxMeta> BoxHeatSources;
	int BoxCount;
	TArray<FCapsuleMeta> CapsuleHeatSources;
	int CapsuleCount;
	
	UVolumeTexture* Noise;
	UTexture2D* ColorStripe;
	float LowCut;
	float HighCut;
	float HalfValueDepth;
};

struct FNightVisionBoostPSParams
{
	UTexture2D* Noise;
	float GameTime;
	float AtmosphereFlickerIntensity;
	float BoostIntensity;
	float ColorStep;
	float NoiseScale;
};

struct FSyntheticApertureParams
{

};

class FSceneVEProcess
{
public:
	// Hook to the SceneViewExtension Base
	static FScreenPassTexture AddThermalProcessPass(
		FRDGBuilder& GraphBuilder,
		const FSceneView& SceneView,
		const FPostProcessMaterialInputs& Inputs,
		const FThermalCSParams& CSInputParameters
		);

	static FScreenPassTexture AddNightVisionBoostPass(
		FRDGBuilder& GraphBuilder,
		const FSceneView& SceneView,
		const FPostProcessMaterialInputs& Inputs,
		const FNightVisionBoostPSParams& Params
		);

	static FScreenPassTexture AddSyntheticAperturePass(
		FRDGBuilder& GraphBuilder,
		const FSceneView& SceneView,
		const FPostProcessMaterialInputs& Inputs,
		const FSyntheticApertureParams& Params
		);
};