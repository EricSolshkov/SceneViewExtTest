#pragma once
#include "CoreMinimal.h"
#include "HeatResource.generated.h"

USTRUCT(Blueprintable)
struct FHeatResource
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Center;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Color;
	
public:
	FHeatResource();
	
	FHeatResource(const FVector& iCenter, const FVector& iColor, float iRadius);
};