#pragma once

#include "../Structure.h"


class StorageTanks : public Structure
{
public:
	StorageTanks();

	StringTable createInspectorViewTable() const override;
};
