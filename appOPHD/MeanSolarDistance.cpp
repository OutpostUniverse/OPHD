#include "MeanSolarDistance.h"

#include <stdexcept>
#include <string>


namespace
{
	float meanSolarDistance = 1;
}


void setMeanSolarDistance(float newMeanSolarDistance)
{
	if (newMeanSolarDistance <= 0)
	{
		throw std::runtime_error("Must set a positive value for `meanSolarDistance`: " + std::to_string(newMeanSolarDistance));
	}
	meanSolarDistance = newMeanSolarDistance;
}


float getMeanSolarDistance()
{
	return meanSolarDistance;
}
