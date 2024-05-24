// Test project fo SceneViewExtension / RDG Shader Basic setup
// Copyright 2021 - 2022 Ossi Luoto
// 
// SceneVEProcess.cpp - Actual RDG hook and Shader Loading

#include "SceneVEProcess.h"
#include "ShaderDeclarations.h"
#include "RenderGraphUtils.h"
#include "RenderGraphResources.h"

namespace
{
	TAutoConsoleVariable<int32> CVarShaderSelector(
		TEXT("r.SceneVETest.Shader"),
		1,
		TEXT("Select shader to use in the post processing \n")
		TEXT(" 0: Vertex & Pixel Shader ;")
		TEXT(" 1: Computer Shader.(default)"),
		ECVF_RenderThreadSafe);
}

// Draw Screen Pass function copied directly from the engine OpenColorIODisplayExtension.cpp
// copy start
namespace
{
	template <typename TSetupFunction>
	void DrawScreenPass(
		FRHICommandListImmediate& RHICmdList,
		const FSceneView& View,
		const FScreenPassTextureViewport& OutputViewport,
		const FScreenPassTextureViewport& InputViewport,
		const FScreenPassPipelineState& PipelineState,
		TSetupFunction SetupFunction)
	{
		PipelineState.Validate();

		const FIntRect InputRect = InputViewport.Rect;
		const FIntPoint InputSize = InputViewport.Extent;
		const FIntRect OutputRect = OutputViewport.Rect;
		const FIntPoint OutputSize = OutputRect.Size();

		RHICmdList.SetViewport(OutputRect.Min.X, OutputRect.Min.Y, 0.0f, OutputRect.Max.X, OutputRect.Max.Y, 1.0f);

		SetScreenPassPipelineState(RHICmdList, PipelineState);

		// Setting up buffers.
		SetupFunction(RHICmdList);

		FIntPoint LocalOutputPos(FIntPoint::ZeroValue);
		FIntPoint LocalOutputSize(OutputSize);
		EDrawRectangleFlags DrawRectangleFlags = EDRF_UseTriangleOptimization;

		DrawPostProcessPass(
			RHICmdList,
			LocalOutputPos.X, LocalOutputPos.Y, LocalOutputSize.X, LocalOutputSize.Y,
			InputRect.Min.X, InputRect.Min.Y, InputRect.Width(), InputRect.Height(),
			OutputSize,
			InputSize,
			PipelineState.VertexShader,
#if ENGINE_MAJOR_VERSION == 5
			View.StereoViewIndex,
#else
			View.StereoPass,
#endif
			false,
			DrawRectangleFlags);
	}
}

// copy end

// utility for using external texture resource
inline FRDGTextureRef RegisterExternalTexture(FRDGBuilder& GraphBuilder, FRHITexture* Texture,
                                              const TCHAR* NameIfUnregistered)
{
	if (FRDGTextureRef FoundTexture = GraphBuilder.FindExternalTexture(Texture))
	{
		return FoundTexture;
	}
	return GraphBuilder.RegisterExternalTexture(CreateRenderTarget(Texture, NameIfUnregistered));
}


FScreenPassTexture FSceneVEProcess::AddThermalProcessPass(
	FRDGBuilder& GraphBuilder,
	const FSceneView& SceneView,
	const FPostProcessMaterialInputs& Inputs,
	const FThermalCSParams& CSInputParameters)
{
	// SceneViewExtension gives SceneView, not ViewInfo so we need to setup some basics ourself
	const FSceneViewFamily& ViewFamily = *SceneView.Family;
	const ERHIFeatureLevel::Type FeatureLevel = SceneView.GetFeatureLevel();

	const FScreenPassTexture& SceneColor = Inputs.Textures[(uint32)EPostProcessMaterialInput::SceneColor];

	if (!SceneColor.IsValid())
	{
		return SceneColor;
	}

	// Here starts the RDG stuff
	RDG_EVENT_SCOPE(GraphBuilder, "ThermalVisionComputePass");
	{
		// Accesspoint to our Shaders
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(ViewFamily.GetFeatureLevel());

		// Setup all the desciptors.
		FRDGTextureDesc OutputDesc;
		{
			OutputDesc = SceneColor.Texture->Desc;

			OutputDesc.Reset();
			OutputDesc.Flags |= TexCreate_UAV;
			OutputDesc.Flags &= ~(TexCreate_RenderTargetable | TexCreate_FastVRAM);

			FLinearColor ClearColor(0., 0., 0., 0.);
			OutputDesc.ClearValue = FClearValueBinding(ClearColor);
		}

		FRDGTextureRef OutputRTTextureRef = GraphBuilder.CreateTexture(
			OutputDesc, TEXT("templateRenderTargetTexture"));

		// Set the shader parameters
		FThermalVisionCS::FParameters* PassParameters = GraphBuilder.AllocParameters<
			FThermalVisionCS::FParameters>();

		// Input is the SceneColor from PostProcess Material Inputs
		PassParameters->SceneTextures = Inputs.SceneTextures.SceneTextures;

		// There are other ways to obtain this information, but for a reference this is a hack to make a FViewInfo from the SceneView that can be very useful
		checkSlow(SceneView.bIsViewInfo);
		const FViewInfo* View = (FViewInfo*)&SceneView;

		// Important! There are many places where UE sets and scales viewport resolution = what kind of scaling you might need to apply to get
		// your UV's right might depend where you place your custom post processing
		// With this example, we are doing the post processing AFTER TAA, which has already scaled up (if needed) the viewport
		// However, at least in UE 5.0 the UniformBuffer is populated with the initial View and Scale, meaning UV's and viewport Size there will be wrong
		// ie. below might work pre post processing chain, but not at this point
		//			FIntPoint PassViewSize = View->ViewRect.Size();
		//			PassParameters->ViewportRect = View->ViewRect;
		// You could also try to use: 			const float ScreenPercentage = ViewFamily.GetPrimaryResolutionFractionUpperBound() * ViewFamily.SecondaryViewFraction;
		// To obtain a scaling factor for UniformViewBuffer UV's, but at least for 5.0.1 this doesn't seem to honor the changing of project settings manual scaling
		// If this is a bug or a wrong approach to scaling, we'll have to see

		// So we use this instead

		// Get the input size (do note that this is not the full extent but the part of the texture containing the SceneColor information)
		FIntPoint PassViewSize = SceneView.UnconstrainedViewRect.Size();
		PassParameters->ViewportRect = SceneView.UnconstrainedViewRect;

		// As the previously mentioned parameters doesn't seem to honor the changes fully, let's just calculate BufferInv from the texture we just got
#if ENGINE_MAJOR_VERSION == 5
			PassParameters->SceneColorBufferInvSize = FVector2f(1.0f / SceneColor.Texture->Desc.Extent.X, 1.0f / SceneColor.Texture->Desc.Extent.Y);
#else
		PassParameters->SceneColorBufferInvSize = FVector2D(1.0f / SceneColor.Texture->Desc.Extent.X,
		                                                    1.0f / SceneColor.Texture->Desc.Extent.Y);
#endif

		// Method to setup common parameters, we use this to pass ViewUniformBuffer data
		// There is a lot of useful stuff in the ViewUniformBuffer, do note that when getting this from the SceneView, a lot them seem to be unpopulated
		FCommonShaderParameters CommonParameters;
		CommonParameters.ViewUniformBuffer = SceneView.ViewUniformBuffer;
		PassParameters->CommonParameters = CommonParameters;

		// Create UAV from Target Texture
		PassParameters->Output = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(OutputRTTextureRef));

		// Create Structured Buffer for HeatResources
		PassParameters->HeatSourceCount = CSInputParameters.HeatSourceCount;
		// use HeatSourceCount, not Array.Num(). Size not equal if HeatSourceCount = 0 when a empty object is added into array.
		FRDGBufferRef HeatSourceBufRef = CreateStructuredBuffer(
			GraphBuilder,
			TEXT("HeatSources"),
			sizeof(FHeatSourceMeta),
			CSInputParameters.HeatSources.Num(),
			CSInputParameters.HeatSources.GetData(),
			CSInputParameters.HeatSources.Num() * sizeof(FHeatSourceMeta)
		);

		// Since HeatResources is read-only for shader, the view of buffer needs to be SRV.
		// if using SRV, declaration in usf should be StructuredBuffer<> instead of RWSB<>
		// RWSB<> is only for UAV.
		FRDGBufferSRVRef HeatSourcesSRV = GraphBuilder.CreateSRV(HeatSourceBufRef);
		PassParameters->HeatSources = HeatSourcesSRV;

		const FMatrix& ViewMatrix = View->ShadowViewMatrices.GetViewMatrix();
		const FVector CameraDirection = ViewMatrix.GetColumn(2);
		PassParameters->CameraDirection = CameraDirection.GetSafeNormal();

		// Pass Noise and SamplerState
		PassParameters->Noise = CSInputParameters.Noise->TextureReference.TextureReferenceRHI;
		PassParameters->NoiseSampler = TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap, AM_Wrap>::GetRHI();

		// Pass ColorStripe and SamplerState
		PassParameters->ColorStripe = CSInputParameters.ColorStripe->TextureReference.TextureReferenceRHI;
		PassParameters->ColorStripeSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Wrap>::GetRHI();

		// Pass control parameters
		PassParameters->LowCut = CSInputParameters.LowCut;
		PassParameters->TemperatureRange = CSInputParameters.TemperatureRange;
		PassParameters->HalfValueDepth = CSInputParameters.HalfValueDepth;

		// Set groupcount and execute pass
		const int32 kDefaultGroupSize = 8;
		FIntPoint GroupSize(kDefaultGroupSize, kDefaultGroupSize);
		FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(PassViewSize, GroupSize);

		TShaderMapRef<FThermalVisionCS> ComputeShader(GlobalShaderMap);

		FComputeShaderUtils::AddPass(
			GraphBuilder,
			RDG_EVENT_NAME("ThermalProcess CS %dx%d", PassViewSize.X, PassViewSize.Y),
			ComputeShader,
			PassParameters,
			GroupCount);

		// One can use a similar method as in the VS/PS version to create a ScreenPassRenderTarget and return with the MoveTemp
		// We are doing here another trick to do a fast copy from the template buffer to the original scenecolor
		// If you are doing multiple passes, technically you should be able to even use the original scenecolor RT as the final output buffer as UAV
		// However, for some reason that seems to work very randomy, so usually with multiple passes, this is the last method I use with multiple Compute Shaders

		AddCopyTexturePass(GraphBuilder, OutputRTTextureRef, SceneColor.Texture);

		return SceneColor;
	}
}

FScreenPassTexture FSceneVEProcess::AddNightVisionBoostPass(
	FRDGBuilder& GraphBuilder,
	const FSceneView& SceneView,
	const FPostProcessMaterialInputs& Inputs,
	const FNightVisionBoostPSParams& Params)
{
	// SceneViewExtension gives SceneView, not ViewInfo so we need to setup some basics ourself
	const FSceneViewFamily& ViewFamily = *SceneView.Family;
	const ERHIFeatureLevel::Type FeatureLevel = SceneView.GetFeatureLevel();

	const FScreenPassTexture& SceneColor = Inputs.Textures[(uint32)EPostProcessMaterialInput::SceneColor];

	if (!SceneColor.IsValid())
	{
		return SceneColor;
	}

	// Vertex/Pixel Shader version

	// Here starts the RDG stuff
	RDG_EVENT_SCOPE(GraphBuilder, "NightVisionBoostPass");
	{
		// Accesspoint to our Shaders
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(ViewFamily.GetFeatureLevel());

		// Template Render Target to use as main output
		FScreenPassRenderTarget templateRenderTarget;

		// Check if this destination is the last one on the post process pipeline
		if (Inputs.OverrideOutput.IsValid())
		{
			templateRenderTarget = Inputs.OverrideOutput;
		}
		else
		// Otherwise make a template RenderTarget
		{
			FRDGTextureDesc OutputDesc = SceneColor.Texture->Desc;
			OutputDesc.Flags |= TexCreate_RenderTargetable;
			FLinearColor ClearColor(0., 0., 0., 0.);
			OutputDesc.ClearValue = FClearValueBinding(ClearColor);

			FRDGTexture* templateRenderTargetTexture = GraphBuilder.CreateTexture(
				OutputDesc, TEXT("templateRenderTargetTexture"));
			templateRenderTarget = FScreenPassRenderTarget(templateRenderTargetTexture, SceneColor.ViewRect,
			                                               ERenderTargetLoadAction::EClear);
		}

		// The Viewport in source and destination might be different
		const FScreenPassTextureViewport SceneColorViewport(SceneColor);
		const FScreenPassTextureViewport tempRenderTargetViewport(templateRenderTarget);

		FScreenPassRenderTarget SceneColorRenderTarget(SceneColor, ERenderTargetLoadAction::ELoad);

		// We need these for the GraphBuilder AddPass
		FRHIBlendState* DefaultBlendState = FScreenPassPipelineState::FDefaultBlendState::GetRHI();
		FRHIDepthStencilState* DepthStencilState = FScreenPassPipelineState::FDefaultDepthStencilState::GetRHI();

		{
			// Get the assigned shaders
			TShaderMapRef<FThermalVisionVS> VertexShader(GlobalShaderMap);
			TShaderMapRef<FNightVisionBoostPS> PixelShader(GlobalShaderMap);

			// Pass the shader parameters
			FNightVisionBoostPSParameters* PassParameters = GraphBuilder.AllocParameters<
				FNightVisionBoostPSParameters>();

			PassParameters->InputTexture = SceneColor.Texture;
			UE_LOG(LogTemp, Warning,TEXT("SVEProcess: Texture Size = [%d, %d]"), PassParameters->InputTexture->Desc.GetSize().X,PassParameters->InputTexture->Desc.GetSize().Y);
			PassParameters->InputSampler = TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap>::GetRHI();
			PassParameters->NoiseTex2D = Params.Noise->TextureReference.TextureReferenceRHI;
			PassParameters->NoiseSampler = TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap>::GetRHI();
			PassParameters->GameTime = Params.GameTime;
			PassParameters->AtmosphereFlickerIntensity = Params.AtmosphereFlickerIntensity;
			PassParameters->BoostIntensity = Params.BoostIntensity;
			PassParameters->ColorStep = Params.ColorStep;
			PassParameters->NoiseScale = Params.NoiseScale;
			PassParameters->RenderTargets[0] = templateRenderTarget.GetRenderTargetBinding();

			// Add Pass by submitting the shaders and parameters to the GraphBuilder that takes care of scheduling it to the Renderthread 
			GraphBuilder.AddPass(
				RDG_EVENT_NAME("NightVisionBoostMainPass"),
				PassParameters,
				ERDGPassFlags::Raster,
				[&SceneView,
					VertexShader,
					PixelShader,
					DefaultBlendState,
					DepthStencilState,
					SceneColorViewport,
					tempRenderTargetViewport,
					PassParameters](FRHICommandListImmediate& RHICmdList)
				{
					DrawScreenPass(
						RHICmdList,
						SceneView,
						tempRenderTargetViewport,
						SceneColorViewport,
						FScreenPassPipelineState(VertexShader, PixelShader, DefaultBlendState, DepthStencilState),
						[&](FRHICommandListImmediate& RHICmdList)
						{
							SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(),
							                    *PassParameters);
						});
				});
		}
		// Return the result
		return MoveTemp(templateRenderTarget);
	}
}

FScreenPassTexture FSceneVEProcess::AddSyntheticAperturePass(FRDGBuilder& GraphBuilder, const FSceneView& SceneView,
                                                             const FPostProcessMaterialInputs& Inputs,
                                                             const FSyntheticApertureParams& Params)
{
	UE_LOG(LogTemp, Warning, TEXT("FSceneVEProcess::AddSyntheticAperturePass is NOT implemented and will do nothing!"))

	const FSceneViewFamily& ViewFamily = *SceneView.Family;
	const ERHIFeatureLevel::Type FeatureLevel = SceneView.GetFeatureLevel();

	const FScreenPassTexture& SceneColor = Inputs.Textures[(uint32)EPostProcessMaterialInput::SceneColor];

	return SceneColor;
}
