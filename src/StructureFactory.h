#pragma once

#include "Common.h"
#include "Constants.h"

#include "Things/Structures/Structures.h"


class StructureFactory
{
public:

	static Structure* get(StructureID type);

	static const ResourcePool& costToBuild(StructureID type);
	static ResourcePool recyclingValue(StructureID type);

protected:

private:

	typedef std::map<StructureID, ResourcePool> StructureCostTable;

private:

	// Explicitly declared private to prevent instantiation.
	StructureFactory() {}
	~StructureFactory() {}

	static void buildCostTable();

private:

	static StructureCostTable	mStructureCostTable;
};