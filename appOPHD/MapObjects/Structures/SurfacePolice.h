#pragma once

#include "../Structure.h"


class SurfacePolice : public Structure
{
public:
	SurfacePolice() : Structure(
		StructureClass::SurfacePolice,
		StructureID::SID_SURFACE_POLICE)
	{
	}
};
