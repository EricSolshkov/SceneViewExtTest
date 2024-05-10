// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ActorComponent.h"
#include "AcThermalSensor.generated.h"


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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UWorld* World;

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
	void SetTemperatureLowCut(float Low);

	UFUNCTION(BlueprintCallable)
	void SetTemperatureHighCut(float High);

	UFUNCTION(BlueprintCallable)
	float GetTemperatureLowCut();

	UFUNCTION(BlueprintCallable)
	float GetTemperatureHighCut();
	
};
