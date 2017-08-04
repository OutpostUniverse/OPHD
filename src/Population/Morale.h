#pragma once

#include <cstdint>

/**
 * Morale modifier values.
 */
class MoraleModifier
{
public:
	MoraleModifier(uint32_t rBonus, uint32_t pBonus, uint32_t fRate, uint32_t mRate) :
		researchBonus(rBonus), productionBonus(pBonus), fertilityRate(fRate), mortalityRate(mRate)
	{}

	MoraleModifier() : researchBonus(0), productionBonus(0), fertilityRate(0), mortalityRate(0)
	{}

public:
	int researchBonus;
	int productionBonus;
	int fertilityRate;
	int mortalityRate;
};
