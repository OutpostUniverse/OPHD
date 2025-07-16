#pragma once

#include "../Structure.h"


class CommTower : public Structure
{
public:
	CommTower(Tile& tile);

	StringTable createInspectorViewTable() const override;
};
