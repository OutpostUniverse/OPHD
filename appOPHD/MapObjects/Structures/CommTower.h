#pragma once

#include "../Structure.h"


class CommTower : public Structure
{
public:
	CommTower();

	int getRange() const;

	StringTable createInspectorViewTable() override;
};
