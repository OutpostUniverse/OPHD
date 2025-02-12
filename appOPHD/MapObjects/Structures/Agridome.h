#pragma once

#include "FoodProduction.h"


class Agridome : public FoodProduction
{
public:
	Agridome();

protected:
	void think() override;

	void disabledStateSet() override;

	virtual int foodCapacity() override;

private:
	virtual int calculateProduction() override;

	bool isStorageFull();
};
