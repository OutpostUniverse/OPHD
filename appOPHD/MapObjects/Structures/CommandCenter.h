#pragma once

#include "FoodProduction.h"


/**
 * Implements the Command Center structure.
 */
class CommandCenter : public FoodProduction
{
public:
	CommandCenter();

	int foodCapacity() override;

	int getRange() const;

protected:
	int calculateProduction() override;
};
