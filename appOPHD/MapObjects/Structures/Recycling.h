#pragma once

#include "../Structure.h"


class Recycling : public Structure
{
public:
	Recycling(Tile& tile);

	int wasteProcessingCapacity() const;
	int residentialSupportCount() const;

	StringTable createInspectorViewTable() const override;
};
