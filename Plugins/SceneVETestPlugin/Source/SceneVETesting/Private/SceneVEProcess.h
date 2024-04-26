// Test project fo SceneExtensionView / RDG Shader Basic setup
// Copyright 2021 Ossi Luoto
// 
// SceneVEProcess.cpp - Actual RDG hook and Shader Loading

#pragma once
#include "SceneVEComponent.h"
#include "ScreenPass.h"

class FSceneVEProcess
{
public:
	// Hook to the SceneViewExtension Base
	static FScreenPassTexture AddSceneVETestPass(
		FRDGBuilder& GraphBuilder,
		const FSceneView& View,
		const FPostProcessMaterialInputs& Inputs,
		const TArray<FHeatResource> HeatResources,
		const UVolumeTexture* Noise
		);
};