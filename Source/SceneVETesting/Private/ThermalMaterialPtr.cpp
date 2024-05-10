#include "ThermalMaterialPtr.h"

UMaterial* ThermalMaterialPtr::Get()
{
	if (!ThermalMaterial)
	{
		ThermalMaterial = LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Material/M_Thermal.M_Thermal'"));
	}
	return ThermalMaterial;
}

UMaterial* ThermalMaterialPtr::ThermalMaterial = nullptr;
