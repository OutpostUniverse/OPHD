#pragma once

#include "FoodProduction.h"


class Agridome : public FoodProduction
{
public:
	Agridome();

protected:
	void think() override;

	void disabledStateSet() override;

	virtual int foodCapacity() const override;

private:
	virtual int calculateProduction() const override;

	bool isStorageFull();
};
