// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ActorComponent.h"

#include "SceneVEComponent.h"

#include "AcThermalSensor.generated.h"



enum EColorStripe
{
	WhiteThermal,
	DarkThermal,
	Inferno,
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCENEVETESTING_API UAcThermalSensor : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAcThermalSensor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlayerCameraManager* CameraManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* ParentMaterial;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInstanceDynamic* MaterialInstance;

	bool IsThermalVisionEnabled;
	
	// this is the way to store the SceneExtensionView to keep it safe from not being destroyed - from: SceneViewExtension.h
	TSharedPtr<class FThermalVisionExt, ESPMode::ThreadSafe > ThermalSVExtension;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void CreateSceneViewExtension();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UVolumeTexture* Noise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ColorStripe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UTexture2D*> ColorStripeTextures;

	TArray<FHeatSourceMeta> HeatSources;

private:
	int CurrentColorStripeIndex = 0;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, CallInEditor)
	void EnableThermalVision();

	UFUNCTION(BlueprintCallable, CallInEditor)
	void DisableThermalVision();

	UFUNCTION(BlueprintCallable, CallInEditor)
	bool GetThermalVisionEnabled();

	UFUNCTION(BlueprintCallable)
	void SetColorStripe(UTexture2D* Tex);

	UFUNCTION(BlueprintCallable)
	void SwitchColorStripe()
	{
		if(ColorStripeTextures.Num() != 0)
		{
			CurrentColorStripeIndex = (CurrentColorStripeIndex + 1) % ColorStripeTextures.Num();
			SetColorStripe(ColorStripeTextures[CurrentColorStripeIndex]);
		}
	}

	UFUNCTION(BlueprintCallable)
	void SetTemperatureLowCut(float Low);

	UFUNCTION(BlueprintCallable)
	void SetTemperatureHighCut(float High);

	UFUNCTION(BlueprintCallable)
	float GetTemperatureLowCut();

	UFUNCTION(BlueprintCallable)
	float GetTemperatureHighCut();

	UFUNCTION(BlueprintCallable)
	void SetHalfValueDepth(float Depth);

	UFUNCTION(CallInEditor, BlueprintCallable)
	void UpdateHeatSources();
	
};
