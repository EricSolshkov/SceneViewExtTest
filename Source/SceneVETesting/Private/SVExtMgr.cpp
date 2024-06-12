// Fill out your copyright notice in the Description page of Project Settings.


#include "SVExtMgr.h"

ASVExtMgr* ASVExtMgr::Instance = nullptr;

// Sets default values
ASVExtMgr::ASVExtMgr()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASVExtMgr::BeginPlay()
{
	Super::BeginPlay();
	if (!Instance)
	{
		Instance = this;
	}
	else
	{
		Destroy();
	}
}

// Called every frame
void ASVExtMgr::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHeatSources();
}

void ASVExtMgr::Destroyed()
{
	Super::Destroyed();
	if (Instance == this) Instance = nullptr;
}

ASVExtMgr* ASVExtMgr::GetInstance(UWorld* World)
{
	if(!Instance)
	{
		Instance = Cast<ASVExtMgr>(World->SpawnActor(ASVExtMgr::StaticClass()));
	}
	return Instance;
}

TSharedPtr<class FIntegratedSVExt, ESPMode::ThreadSafe > ASVExtMgr::GetExt()
{
	if(!SVExt)
	{
		CreateSceneViewExtension();
	}
	return SVExt;
}

// On a separate function to hook f.ex. for in editor creation etc.
void ASVExtMgr::CreateSceneViewExtension()
{
	SVExt = FSceneViewExtensions::NewExtension<FIntegratedSVExt>();
	check(SVExt)
	VolumeNoise = LoadObject<UVolumeTexture>(nullptr, TEXT("VolumeTexture'/SceneVETestPlugin/VT_PerlinWorley_Balanced.VT_PerlinWorley_Balanced'"));
	check(VolumeNoise);
	Noise2D = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Engine/EngineMaterials/Good64x64TilingNoiseHighFreq.Good64x64TilingNoiseHighFreq'"));
	check(Noise2D);
	SVExt->VolumeNoise = VolumeNoise;
	SVExt->Noise2D = Noise2D;
	UE_LOG(LogTemp, Log, TEXT("ASVExtMgr: Scene Extension Created!"));
}

void ASVExtMgr::UpdateHeatSources()
{
	HeatSources.Empty();
	
	TArray<UAcThermalManager*> ThmMgrs;
	
	// iterate through all actor, get all heat source components of every actor
	for(TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		TArray<UAcThermalManager*> Comps;
		(*ActorItr)->GetComponents(Comps);
		if(Comps.Num() > 0)
		{
			ThmMgrs.Append(Comps);
		}
	}
	for (auto ThmMgr : ThmMgrs)
	{
		ThmMgr->AppendHeatSources(HeatSources);
	}
	SVExt->HeatSources = HeatSources;
}

