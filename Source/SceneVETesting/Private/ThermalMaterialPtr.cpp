#include "ThermalMaterialPtr.h"

UMaterial* ThermalMaterialPtr::Get()
{
	if (!ThermalMaterial)
	{
		ThermalMaterial = LoadObject<UMaterial>(nullptr, TEXT("Material'/Plugin/SceneVETestPlugin/Content/M_Thermal.M_Thermal'"));
	}
	return ThermalMaterial;
}

UMaterial* ThermalMaterialPtr::ThermalMaterial = nullptr;
