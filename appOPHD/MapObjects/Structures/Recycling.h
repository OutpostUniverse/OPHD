#pragma once

#include "../Structure.h"


class Recycling : public Structure
{
public:
	Recycling();

	virtual int wasteProcessingCapacity() const;
	virtual int residentialSupportCount() const;

	StringTable createInspectorViewTable() const override;
};
