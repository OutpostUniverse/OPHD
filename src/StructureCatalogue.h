#pragma once

#include "Common.h"
#include "Constants.h"

#include "Things/Structures/Structures.h"


/** 
 * \class	StructureCatalogue
 * \brief	Provides a means of instantiating new structures and getting build
 *			cost / recycle value / population requirements.
 * 
 * StructureCatalogue is implemented as a static class and should never be
 * instantiated.
 * 
 * \note	StructureCatalogue::init() must be called prior to use.
 * 
 * \code{.cpp}
 * ResourcePool _rp = StructureCatalogue::costToBuild(SID_AGRIDOME);
 * PopulationRequirements _pr = StructureCatalogue::populationRequirements(SID_AGRIDOME);
 * \endcode
 */
class StructureCatalogue
{

public:
	static void init();

	static Structure* get(StructureID type);

	static const PopulationRequirements& populationRequirements(StructureID type);
	static const ResourcePool& costToBuild(StructureID type);
	static const ResourcePool& recyclingValue(StructureID type);

	static bool canBuild(const ResourcePool& source, StructureID type);

private:
	StructureCatalogue() {}	// Explicitly declared private to prevent instantiation.
	~StructureCatalogue() {}	// Explicitly declared private to prevent instantiation.

	static void buildCostTable();
	static void buildPopulationRequirementsTable();
	static void buildRecycleValueTable();
	
	static ResourcePool recycleValue(StructureID type, float percent);

private:
	//static vector<ResourcePool> mStructureCostTable;
	static std::array<ResourcePool, SID_COUNT> mStructureCostTable;
	static std::array<ResourcePool, SID_COUNT> mStructureRecycleValueTable;
	static std::array<PopulationRequirements, SID_COUNT> mPopulationRequirementsTable;
};