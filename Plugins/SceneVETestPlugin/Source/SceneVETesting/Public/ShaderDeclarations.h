#pragma once
#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

#include "RenderGraphUtils.h"

// Parameter Declaration
BEGIN_SHADER_PARAMETER_STRUCT(FSceneVETestShaderParameters, )
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
	SHADER_PARAMETER_SAMPLER(SamplerState, InputSampler)
	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

// Declare Test Shader Class

class SCENEVETESTING_API FSceneVETestShaderVS : public FGlobalShader
{
public:
	// Vertex Shader Declaration
	DECLARE_GLOBAL_SHADER(FSceneVETestShaderVS)

	// Basic shader stuff
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	FSceneVETestShaderVS() {}

	FSceneVETestShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}
};

class SCENEVETESTING_API FSceneVETestShaderPS : public FGlobalShader
{
public:
	// RDG Pixel Shader Declaration
	DECLARE_GLOBAL_SHADER(FSceneVETestShaderPS)
	SHADER_USE_PARAMETER_STRUCT_WITH_LEGACY_BASE(FSceneVETestShaderPS, FGlobalShader)

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
	using FParameters = FSceneVETestShaderParameters;

};

// Struct to include common parameters, useful when doing multiple shaders
BEGIN_SHADER_PARAMETER_STRUCT(FCommonShaderParameters, )
	SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, ViewUniformBuffer)
END_SHADER_PARAMETER_STRUCT()

class SCENEVETESTING_API FSceneVETestShaderCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSceneVETestShaderCS)
	SHADER_USE_PARAMETER_STRUCT(FSceneVETestShaderCS, FGlobalShader)

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FCommonShaderParameters, CommonParameters)
		SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTextures)
		SHADER_PARAMETER(FIntRect, ViewportRect)
#if ENGINE_MAJOR_VERSION == 5
		SHADER_PARAMETER(FVector2f, SceneColorBufferInvSize)
#else
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FHeatResource>, HeatResources)
		SHADER_PARAMETER(uint32, HeatResourceCount)
		SHADER_PARAMETER(FVector2D, SceneColorBufferInvSize)

		SHADER_PARAMETER_TEXTURE(Texture3D, Noise)
		SHADER_PARAMETER_SAMPLER(SamplerState, NoiseSampler)
	
#endif
		
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, Output)
	END_SHADER_PARAMETER_STRUCT()

	// Basic shader stuff
		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

// Shader implementation Macro doesn't work on .h file so load them here
IMPLEMENT_GLOBAL_SHADER(FSceneVETestShaderPS, "/Plugins/SceneVETestPlugin/SceneVETestShaderPS.usf", "MainPS", SF_Pixel);
// point to the shader  file, name of the main function in shader.usf
IMPLEMENT_GLOBAL_SHADER(FSceneVETestShaderVS, "/Plugins/SceneVETestPlugin/SceneVETestShaderVS.usf", "MainVS", SF_Vertex)
; // point to the shader  file, name of the main function in shader.usf
IMPLEMENT_GLOBAL_SHADER(FSceneVETestShaderCS, "/Plugins/SceneVETestPlugin/SceneVETestShaderCS.usf", "MainCS",
						SF_Compute); // point to the shader  file, name of the main function in shader.usf