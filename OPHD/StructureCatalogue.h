#pragma once

#include "Common.h"
#include "Constants.h"
#include "Things/Structures/Structures.h"
#include "StorableResources.h"

#include <array>
#include <map>

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
	static void init(float meanSolarDistance);

	static Structure* get(StructureID type);

	static const PopulationRequirements& populationRequirements(StructureID type);
	static const StorableResources& costToBuild(StructureID type);
	static const StorableResources& recyclingValue(StructureID type);

	static bool canBuild(const StorableResources& source, StructureID type);

private:
	StructureCatalogue() {} // Explicitly declared private to prevent instantiation.
	~StructureCatalogue() {} // Explicitly declared private to prevent instantiation.

	static std::map<StructureID, StorableResources> buildRecycleValueTable();

	static StorableResources recycleValue(StructureID type, int percent);

private:
	static std::map<StructureID, StorableResources> mStructureRecycleValueTable;
	static float mMeanSolarDistance;
};
