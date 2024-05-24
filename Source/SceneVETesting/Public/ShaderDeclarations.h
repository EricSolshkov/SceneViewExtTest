#pragma once
#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

#include "RenderGraphUtils.h"

// Struct to include common parameters, useful when doing multiple shaders
BEGIN_SHADER_PARAMETER_STRUCT(FCommonShaderParameters, )
	SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, ViewUniformBuffer)
END_SHADER_PARAMETER_STRUCT()

// Parameter Declaration
BEGIN_SHADER_PARAMETER_STRUCT(FThermalVisionPSParameters, )
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
	SHADER_PARAMETER_SAMPLER(SamplerState, InputSampler)
	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

BEGIN_SHADER_PARAMETER_STRUCT(FNightVisionBoostPSParameters, )
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
	SHADER_PARAMETER_SAMPLER(SamplerState, InputSampler)
	SHADER_PARAMETER_TEXTURE(Texture2D, NoiseTex2D)
	SHADER_PARAMETER_SAMPLER(SamplerState, NoiseSampler)
	SHADER_PARAMETER(float, GameTime)
	SHADER_PARAMETER(float, AtmosphereFlickerIntensity)
	SHADER_PARAMETER(float, BoostIntensity)
	SHADER_PARAMETER(float, ColorStep)
	SHADER_PARAMETER(float, NoiseScale)
	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

// Declare Shader Classes

class SCENEVETESTING_API FThermalVisionVS : public FGlobalShader
{
public:
	// Vertex Shader Declaration
	DECLARE_GLOBAL_SHADER(FThermalVisionVS)

	// Basic shader stuff
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	FThermalVisionVS() {}

	FThermalVisionVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}
};

class SCENEVETESTING_API FThermalVisionPS : public FGlobalShader
{
public:
	// RDG Pixel Shader Declaration
	DECLARE_GLOBAL_SHADER(FThermalVisionPS)
	SHADER_USE_PARAMETER_STRUCT_WITH_LEGACY_BASE(FThermalVisionPS, FGlobalShader)

	// Basic shader stuff
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	// Use the parameters from previously delcared struct
	using FParameters = FThermalVisionPSParameters;

};

class SCENEVETESTING_API FThermalVisionCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FThermalVisionCS)
	SHADER_USE_PARAMETER_STRUCT(FThermalVisionCS, FGlobalShader)

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FCommonShaderParameters, CommonParameters)
		SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTextures)
		SHADER_PARAMETER(FIntRect, ViewportRect)
#if ENGINE_MAJOR_VERSION == 5
		SHADER_PARAMETER(FVector2f, SceneColorBufferInvSize)
#else
		SHADER_PARAMETER(FVector2D, SceneColorBufferInvSize)
	
#endif
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FHeatSourceMeta>, HeatSources)
		SHADER_PARAMETER(uint32, HeatSourceCount)
		SHADER_PARAMETER(float, LowCut)
		SHADER_PARAMETER(float, TemperatureRange)
		SHADER_PARAMETER(float, HalfValueDepth)
		SHADER_PARAMETER(FVector, CameraDirection)

		SHADER_PARAMETER_TEXTURE(Texture3D, Noise)
		SHADER_PARAMETER_SAMPLER(SamplerState, NoiseSampler)

		SHADER_PARAMETER_TEXTURE(Texture2D, ColorStripe)
		SHADER_PARAMETER_SAMPLER(SamplerState, ColorStripeSampler)
	
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, Output)
	
	END_SHADER_PARAMETER_STRUCT()

	// Basic shader stuff
		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

class SCENEVETESTING_API FNightVisionBoostPS : public FGlobalShader
{
public:
	// RDG Pixel Shader Declaration
	DECLARE_GLOBAL_SHADER(FNightVisionBoostPS)
	SHADER_USE_PARAMETER_STRUCT_WITH_LEGACY_BASE(FNightVisionBoostPS, FGlobalShader)

	// Basic shader stuff
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	// Use the parameters from previously delcared struct
	using FParameters = FNightVisionBoostPSParameters;

};

// Shader implementation Macro doesn't work on .h file so load them here
IMPLEMENT_GLOBAL_SHADER(FThermalVisionPS, "/Plugins/SceneVETestPlugin/ThermalVisionPS.usf", "MainPS", SF_Pixel); // point to the shader  file, name of the main function in shader.usf
IMPLEMENT_GLOBAL_SHADER(FThermalVisionVS, "/Plugins/SceneVETestPlugin/ThermalVisionVS.usf", "MainVS", SF_Vertex); // point to the shader  file, name of the main function in shader.usf
IMPLEMENT_GLOBAL_SHADER(FThermalVisionCS, "/Plugins/SceneVETestPlugin/ThermalVisionCS.usf", "MainCS", SF_Compute); // point to the shader  file, name of the main function in shader.usf

IMPLEMENT_GLOBAL_SHADER(FNightVisionBoostPS, "/Plugins/SceneVETestPlugin/NightVisionBoostPS.usf", "MainPS", SF_Pixel); // point to the shader  file, name of the main function in shader.usf
