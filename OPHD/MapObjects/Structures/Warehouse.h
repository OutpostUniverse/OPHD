#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"
#include "../../ProductPool.h"


class Warehouse : public Structure
{
public:
	Warehouse() : Structure(
		StructureClass::Warehouse,
		StructureID::SID_WAREHOUSE)
	{
	}

	ProductPool& products() { return mProducts; }

private:
	ProductPool mProducts;
};
