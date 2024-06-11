﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeatSource.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Engine/Private/Materials/MaterialInstanceSupport.h"

#include "AcThermalManager.generated.h"

//Component Thermal Manager Manages relative material and HeatSources of the attached actor.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCENEVETESTING_API UAcThermalManager : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAcThermalManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SurfaceTemperature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AHeatSourceBase*> HeatSources;
	
	bool ThermalRenderingEnabled;

	float TemperatureLowCut;

	float TemperatureHighCut;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> ThermalMaterialInstances;

	UPROPERTY()
	UMeshComponent* MeshComponent;

	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials;

	UPROPERTY()
	TArray<UTexture2D*> OriginalTexes;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintSetter, CallInEditor)
	float SetSurfaceTemperature(const float NewTemperature)
	{
		SurfaceTemperature = NewTemperature;
		return SurfaceTemperature;
	}

	UFUNCTION(BlueprintGetter, CallInEditor)
	float GetSurfaceTemperature() const
	{
		return SurfaceTemperature;
	}

	UFUNCTION(BlueprintCallable)
	void SetTemperatureHighCut(const float NewTemperature);

	UFUNCTION(BlueprintCallable)
	float GetTemperatureHighCut();

	UFUNCTION(BlueprintCallable)
	void SetTemperatureLowCut(const float NewTemperature);

	UFUNCTION(BlueprintCallable)
	float GetTemperatureLowCut();

	// ThermalMgr设置Enable和Disable的意义在于切换专用热成像材质。
	UFUNCTION(BlueprintCallable)
	void EnableThermalRendering();

	UFUNCTION(BlueprintCallable)
	void DisableThermalRendering();

	UFUNCTION(BlueprintCallable)
	bool GetThermalRenderingStatus();

	UFUNCTION(BlueprintCallable)
	void AppendHeatSources(TArray<AHeatSourceBase*>& Container);

	UFUNCTION(BlueprintCallable)
	static UAcThermalManager* Create(AActor* Actor,float Temperature, bool Enabled);

	UFUNCTION(BlueprintCallable, CallInEditor)
	void AddSphere();
	
	UFUNCTION(BlueprintCallable, CallInEditor)
	void AddBox();
	
	UFUNCTION(BlueprintCallable, CallInEditor)
	void AddCapsule();
};
