#pragma once

#include <cstdint>

/**
 * Morale modifier values.
 */
struct MoraleModifier
{
public:
	MoraleModifier() = default;
	MoraleModifier(int rBonus, int pBonus, int fRate, int mRate) :
		researchBonus(rBonus), productionBonus(pBonus), fertilityRate(fRate), mortalityRate(mRate)
	{}

	int researchBonus{0};
	int productionBonus{0};
	int fertilityRate{0};
	int mortalityRate{0};
};
