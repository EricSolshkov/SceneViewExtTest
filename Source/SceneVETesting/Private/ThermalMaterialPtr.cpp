#include "ThermalMaterialPtr.h"

inline UMaterial* FThermalMaterialPtr::Get()
{
	return LoadObject<UMaterial>(nullptr, TEXT("Material'/SceneVETestPlugin/M_Thermal.M_Thermal'"));
}
