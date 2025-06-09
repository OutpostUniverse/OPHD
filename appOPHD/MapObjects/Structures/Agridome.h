#pragma once

#include "FoodProduction.h"


class Agridome : public FoodProduction
{
public:
	Agridome();

protected:
	void think() override;

	void disabledStateSet() override;

private:
	virtual int calculateProduction() const override;

	bool isStorageFull();
};
