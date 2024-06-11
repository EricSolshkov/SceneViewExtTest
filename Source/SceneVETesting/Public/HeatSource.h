// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HeatSource.generated.h"

// 定义枚举类型
UENUM(BlueprintType)
enum class EHeatSourceShapeType : uint8
{
	None UMETA(DisplayName = "None"),
	Sphere UMETA(DisplayName = "Sphere"),
	Box UMETA(DisplayName = "Box"),
	Capsule UMETA(DisplayName = "Capsule")
};

struct FSphereMeta
{
	FMatrix WorldToLocal;
	float Radius;
	float Temperature;
	float Thickness;

	FSphereMeta() : WorldToLocal(FMatrix::Identity), Radius(0), Temperature(0), Thickness(1) {}
};

struct FBoxMeta
{
	FMatrix WorldToLocal;
	FVector XYZ;
	float Temperature;
	float Thickness;

	FBoxMeta() : WorldToLocal(FMatrix::Identity), XYZ(FVector(0, 0, 0)), Temperature(0), Thickness(1) {}
};

struct FCapsuleMeta
{
	FMatrix WorldToLocal;
	float Radius;
	float HalfHeight;
	float Temperature;
	float Thickness;

	FCapsuleMeta() : WorldToLocal(FMatrix::Identity), Radius(0), HalfHeight(0), Temperature(0), Thickness(1) {}
};

UCLASS(Abstract)
class SCENEVETESTING_API AHeatSourceBase : public AActor{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Temperature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Thickness;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnTime;

	UPROPERTY()
	EHeatSourceShapeType Shape;
	
	// Sets default values for this actor's properties
	AHeatSourceBase();

	explicit AHeatSourceBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure)
	virtual float GetTemperature() const { return Temperature; };

	UFUNCTION(BlueprintPure)
	virtual float SetTemperature(const float Val) { Temperature = Val; return Val; };

	// for editor debug
#if WITH_EDITOR
	virtual void Tick(float DeltaTime) override;
	
	virtual bool ShouldTickIfViewportsOnly() const override { return true; };

	virtual void DrawDebugShape();
#endif
};

UCLASS(Blueprintable)
class SCENEVETESTING_API ASphereHeatSource : public AHeatSourceBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;
	
	ASphereHeatSource();

	explicit ASphereHeatSource(const FObjectInitializer& ObjectInitializer);
	
#if WITH_EDITOR
	virtual void DrawDebugShape() override;
#endif

	FSphereMeta GetMeta() const;
};

UCLASS(Blueprintable)
class SCENEVETESTING_API ABoxHeatSource : public AHeatSourceBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector XYZ;

	ABoxHeatSource();
	
	explicit ABoxHeatSource(const FObjectInitializer& ObjectInitializer);
	
#if WITH_EDITOR
	virtual void DrawDebugShape() override;
#endif

	FBoxMeta GetMeta() const;
};

UCLASS(Blueprintable)
class SCENEVETESTING_API ACapsuleHeatSource : public AHeatSourceBase
{
	GENERATED_BODY()

public:
	ACapsuleHeatSource();

	explicit ACapsuleHeatSource(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HalfHeight;

#if WITH_EDITOR
	virtual void DrawDebugShape() override;
#endif

	FCapsuleMeta GetMeta() const;
};
