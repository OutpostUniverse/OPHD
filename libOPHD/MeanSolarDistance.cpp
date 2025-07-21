#include "MeanSolarDistance.h"

#include <stdexcept>
#include <string>


namespace
{
	float meanSolarDistance = 1;

	constexpr int solarEffectivenessScale = 256;
	int solarEffectiveness = solarEffectivenessScale;
}


void setMeanSolarDistance(float newMeanSolarDistance)
{
	if (newMeanSolarDistance <= 0)
	{
		throw std::runtime_error("Must set a positive value for `meanSolarDistance`: " + std::to_string(newMeanSolarDistance));
	}
	meanSolarDistance = newMeanSolarDistance;
	solarEffectiveness = static_cast<int>(solarEffectivenessScale / meanSolarDistance);
}


float getMeanSolarDistance()
{
	return meanSolarDistance;
}


int scaleSolarOutput(int baseOutput)
{
	return baseOutput * solarEffectiveness / solarEffectivenessScale;
}
