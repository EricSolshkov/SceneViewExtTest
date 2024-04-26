#include "HeatResource.h"

FHeatResource::FHeatResource()
{
	Center = FVector(0,0,0);
	Radius = 0;
	Color = FVector(0,0,0);
}

FHeatResource::FHeatResource(const FVector& iCenter, const FVector& iColor, float iRadius)
{
	Center = iCenter;
	Radius = iRadius;
	Color = iColor;
}
