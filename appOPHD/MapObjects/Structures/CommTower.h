#pragma once

#include "../Structure.h"


class CommTower : public Structure
{
public:
	CommTower();

	StringTable createInspectorViewTable() const override;
};
