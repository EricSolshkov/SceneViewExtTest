// Test project fo SceneExtensionView / RDG Shader Basic setup
// Copyright 2021 Ossi Luoto
// 
// SceneVEProcess.cpp - Actual RDG hook and Shader Loading

#pragma once
#include "SceneVEComponent.h"
#include "ScreenPass.h"

struct FThermalCSParams
{
	TArray<FHeatSourceMeta> HeatSources;
	int HeatSourceCount;
	UVolumeTexture* Noise;
	UTexture2D* ColorStripe;
	float LowCut;
	float TemperatureRange;
	float HalfValueDepth;
};

struct FNightVisionBoostPSParams
{
	float BoostIntensity;
	float Jitter;
	float ColorStep;
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
		const FNightVisionBoostPSParams PSInputParameters
		);

	static FScreenPassTexture AddSyntheticAperturePass(
		FRDGBuilder& GraphBuilder,
		const FSceneView& SceneView,
		const FPostProcessMaterialInputs& Inputs,
		const FSyntheticApertureParams Params
		);
};