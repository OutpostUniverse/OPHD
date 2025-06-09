#pragma once

#include "FoodProduction.h"


class CommandCenter : public FoodProduction
{
public:
	CommandCenter();

protected:
	int calculateProduction() const override;
};
