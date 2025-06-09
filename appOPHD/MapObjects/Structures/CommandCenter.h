#pragma once

#include "FoodProduction.h"


/**
 * Implements the Command Center structure.
 */
class CommandCenter : public FoodProduction
{
public:
	CommandCenter();

protected:
	int calculateProduction() const override;
};
