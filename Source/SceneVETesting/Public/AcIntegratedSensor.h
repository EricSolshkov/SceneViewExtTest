// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ActorComponent.h"

#include "SceneVEComponent.h"

#include "AcIntegratedSensor.generated.h"



enum EColorStripe
{
	WhiteThermal,
	DarkThermal,
	Inferno,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCENEVETESTING_API UAcIntegratedSensor : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAcIntegratedSensor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlayerCameraManager* CameraManager;
	
	// this is the way to store the SceneExtensionView to keep it safe from not being destroyed - from: SceneViewExtension.h
	TSharedPtr<class FIntegratedSVExt, ESPMode::ThreadSafe > SVExt;

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

	UFUNCTION(BlueprintCallable)
	void EnableNightVisionBoost();

	UFUNCTION(BlueprintCallable)
	void DisableNightVisionBoost();

	UFUNCTION(BlueprintCallable)
	bool GetNightVisionBoostEnabled() const
	{
		return (SVExt->GetEnabledSensor() == NightVisionBoost);
	}
};
