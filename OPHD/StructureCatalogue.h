#pragma once

#include "Common.h"
#include "StorableResources.h"

#include <map>


class Structure;
struct PopulationRequirements;


/** 
 * Provides a means of instantiating new structures and getting build
 *			cost / recycle value / population requirements.
 * 
 * StructureCatalogue is implemented as a static class and should never be
 * instantiated.
 * 
 * \note	StructureCatalogue::init() must be called prior to use.
 * 
 */
class StructureCatalogue
{
public:
	StructureCatalogue() = delete;

	static void init(float meanSolarDistance);

	static Structure* get(StructureID type);

	static const PopulationRequirements& populationRequirements(StructureID type);
	static const StorableResources& costToBuild(StructureID type);
	static const StorableResources& recyclingValue(StructureID type);

	static bool canBuild(const StorableResources& source, StructureID type);

private:
	static float mMeanSolarDistance;
};
