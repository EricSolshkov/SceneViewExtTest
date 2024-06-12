// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "IntegratedSVExt.h"
#include "AcThermalManager.h"
#include "SVExtMgr.generated.h"

UCLASS()
class SCENEVETESTING_API ASVExtMgr : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASVExtMgr();

	UPROPERTY(BlueprintReadWrite)
	UVolumeTexture* VolumeNoise;

	UPROPERTY(BlueprintReadWrite)
	UTexture2D* Noise2D;

	UPROPERTY()
	TArray<AHeatSourceBase*> HeatSources;

private:
	// this is the way to store the SceneExtensionView to keep it safe from not being destroyed - from: SceneViewExtension.h
	TSharedPtr<class FIntegratedSVExt, ESPMode::ThreadSafe > SVExt;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void CreateSceneViewExtension();

	void UpdateHeatSources();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;
	
	static ASVExtMgr* GetInstance(UWorld* World);
	
	TSharedPtr<class FIntegratedSVExt, ESPMode::ThreadSafe > GetExt();

private:
	static ASVExtMgr* Instance;

#if WITH_EDITOR
	virtual bool ShouldTickIfViewportsOnly() const override { return true; };
#endif
};
