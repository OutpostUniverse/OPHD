#include "CrimeRateUpdate.h"
#include "../UI/PopulationPanel.h"
#include "../Things/Structures/Structure.h"
#include "../StructureManager.h"
#include <NAS2D/Utility.h>

bool isProtectedByPolice(const std::vector<TileList>& policeOverlays, Structure* structure);
int calculateMoraleChange(int meanCrimeRate);
void setPopulationPanel(int moraleChange, int meanCrimeRate, PopulationPanel& populationPanel);


int CrimeRateUpdate::update(const std::vector<TileList>& policeOverlays, PopulationPanel& populationPanel)
{
	const auto& structuresWithCrime = NAS2D::Utility<StructureManager>::get().structuresWithCrime();

	// Colony will not have a crime rate until at least one structure that supports crime is built
	if (structuresWithCrime.empty()) {
		setPopulationPanel(0, 0, populationPanel);
		return 0;
	}

	double accumulatedCrime = 0;

	for (auto structure : structuresWithCrime)
	{
		int crimeRateChange = isProtectedByPolice(policeOverlays, structure) ? -1 : 1;
		structure->increaseCrimeRate(crimeRateChange);

		accumulatedCrime += structure->crimeRate();
	}

	int meanCrimeRate = static_cast<int>(accumulatedCrime / structuresWithCrime.size());
	int moraleChange = calculateMoraleChange(meanCrimeRate);

	setPopulationPanel(moraleChange, meanCrimeRate, populationPanel);

	return moraleChange;
}


bool isProtectedByPolice(const std::vector<TileList>& policeOverlays, Structure* structure)
{
	const auto& structureTile = NAS2D::Utility<StructureManager>::get().tileFromStructure(structure);

	for (const auto& tile : policeOverlays[structureTile.depth()])
	{
		if (tile->position() == structureTile.position())
		{
			return true;
		}
	}

	return false;
}


int calculateMoraleChange(int meanCrimeRate)
{
	if (meanCrimeRate > 50)
	{
		// Reduce morale by 1 for every 10% above 50%
		return -1 * (meanCrimeRate / 10 - 4);
	}
	else if (meanCrimeRate < 10)
	{
		return 1;
	}

	return 0;
}


void setPopulationPanel(int moraleChange, int meanCrimeRate, PopulationPanel& populationPanel)
{
	populationPanel.crimeRate(meanCrimeRate);

	if (moraleChange > 0)
	{
		populationPanel.addMoraleReason("Low Crime Rate", moraleChange);
	}
	else if (moraleChange < 0)
	{
		populationPanel.addMoraleReason("High Crime Rate", moraleChange);
	}
}
