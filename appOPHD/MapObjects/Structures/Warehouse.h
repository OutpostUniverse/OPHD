#pragma once

#include "../Structure.h"

#include "../../ProductPool.h"


class Warehouse : public Structure
{
public:
	Warehouse(Tile& tile) :
		Structure{StructureClass::Warehouse, StructureID::SID_WAREHOUSE, tile}
	{
	}

	ProductPool& products() { return mProducts; }
	const ProductPool& products() const { return mProducts; }

private:
	ProductPool mProducts;
};
