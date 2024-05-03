// Test project fo SceneExtensionView / RDG Shader Basic setup
// Copyright 2021 Ossi Luoto
// 
// SceneVEProcess.cpp - Actual RDG hook and Shader Loading

#pragma once
#include "SceneVEComponent.h"
#include "ScreenPass.h"

struct MyComputeShaderInputParameters
{
	TArray<FHeatResource> HeatResources;
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
	static FScreenPassTexture AddSceneVETestPass(
		FRDGBuilder& GraphBuilder,
		const FSceneView& View,
		const FPostProcessMaterialInputs& Inputs,
		const MyComputeShaderInputParameters CSInputParameters
		);
};