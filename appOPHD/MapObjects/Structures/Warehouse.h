#pragma once

#include "../Structure.h"

#include "../../ProductPool.h"

#include <libOPHD/EnumStructureID.h>


class Warehouse : public Structure
{
public:
	Warehouse(Tile& tile) :
		Structure{StructureID::SID_WAREHOUSE, tile}
	{
	}

	ProductPool& products() { return mProducts; }
	const ProductPool& products() const { return mProducts; }

private:
	ProductPool mProducts;
};
