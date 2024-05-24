// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HeatSource.generated.h"

// 定义枚举类型
UENUM(BlueprintType)
enum class EHeatSourceShapeType : uint8
{
	Sphere UMETA(DisplayName = "Sphere"),
	Box UMETA(DisplayName = "Box"),
	Capsule UMETA(DisplayName = "Capsule")
};

USTRUCT(Blueprintable)
struct FHeatSourceMeta
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Center;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Temperature;
	
public:
	FHeatSourceMeta();
	
	FHeatSourceMeta(const FVector& iCenter, float iRadius, float iTemperature);
};


UCLASS()
class SCENEVETESTING_API AHeatSource : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHeatSource();

	// Called on construction/spawn/property changed.
	void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Temperature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHeatSourceShapeType ShapeType = EHeatSourceShapeType::Sphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* AttachedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* MC_Shape;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FHeatSourceMeta GetMeta(float LowCut, float HighCut);
};
