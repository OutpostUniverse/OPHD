#pragma once

#include "../Structure.h"


class Recycling : public Structure
{
public:
	Recycling(Tile& tile);

	virtual int wasteProcessingCapacity() const;
	virtual int residentialSupportCount() const;

	StringTable createInspectorViewTable() const override;
};
