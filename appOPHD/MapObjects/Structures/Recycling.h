#pragma once

#include "../Structure.h"


class Recycling : public Structure
{
public:
	Recycling(Tile& tile);

	StringTable createInspectorViewTable() const override;
};
