#pragma once

#include "FoodProduction.h"


class CommandCenter : public FoodProduction
{
public:
	CommandCenter(Tile& tile);

protected:
	int calculateProduction() const override;
};
