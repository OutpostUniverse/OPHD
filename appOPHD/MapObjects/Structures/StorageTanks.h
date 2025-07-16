#pragma once

#include "../Structure.h"


class StorageTanks : public Structure
{
public:
	StorageTanks(Tile& tile);

	StringTable createInspectorViewTable() const override;
};
