// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeatSource.h"
#include "ThermalMaterialPtr.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetMathLibrary.h"
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
	TArray<AHeatSource*> HeatSources;
	
	bool ThermalRenderingEnabled; 

	UMaterialInstanceDynamic* ThermalMaterialInstance;

	UMeshComponent* MeshComponent;

	TArray<UMaterialInterface*> OriginalMaterials;

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
	float GetSurfaceTemperature()
	{
		return SurfaceTemperature;
	}

	// ThermalMgr设置Enable和Disable的意义在于切换专用热成像材质。
	UFUNCTION(BlueprintCallable, CallInEditor)
	void EnableThermalRendering();

	UFUNCTION(BlueprintCallable, CallInEditor)
	void DisableThermalRendering();

	UFUNCTION(BlueprintCallable)
	bool GetThermalRenderingStatus();

	UFUNCTION(BlueprintCallable)
	void AppendHeatSourcesMeta(TArray<FHeatSourceMeta>& Container);

	UFUNCTION(BlueprintCallable)
	static UAcThermalManager* Create(AActor* Actor,float Temperature, bool Enabled);
};
