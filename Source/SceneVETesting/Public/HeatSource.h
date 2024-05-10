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
	Box UMETA(DisplayName = "Box")
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
	FVector Color;
	
public:
	FHeatSourceMeta();
	
	FHeatSourceMeta(const FVector& iCenter, const FVector& iColor, float iRadius);
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Temperature;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float SpawnTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EHeatSourceShapeType ShapeType = EHeatSourceShapeType::Sphere;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float SpawnSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AActor* AttachedActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* MC_Shape;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FHeatSourceMeta GetMeta();
};
