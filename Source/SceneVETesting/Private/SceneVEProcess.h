// Test project fo SceneExtensionView / RDG Shader Basic setup
// Copyright 2021 Ossi Luoto
// 
// SceneVEProcess.cpp - Actual RDG hook and Shader Loading

#pragma once
#include "SceneVEComponent.h"
#include "ScreenPass.h"

struct FCSInputParameters
{
	TArray<FHeatSourceMeta> HeatSources;
	int HeatSourceCount;
	UVolumeTexture* Noise;
	UTexture2D* ColorStripe;
	float LowCut;
	float TemperatureRange;
	float HalfValueDepth;
};

class FSceneVEProcess
{
public:
	// Hook to the SceneViewExtension Base
	static FScreenPassTexture AddThermalProcessPass(
		FRDGBuilder& GraphBuilder,
		const FSceneView& View,
		const FPostProcessMaterialInputs& Inputs,
		const FCSInputParameters CSInputParameters
		);
};