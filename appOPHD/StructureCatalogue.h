#pragma once

#include <libOPHD/EnumStructureID.h>

#include <string>


class Structure;
class Tile;
struct StructureType;
struct StorableResources;


/**
 * Provides a means of instantiating new structures and getting build
 *			cost / recycle value / population requirements.
 *
 * StructureCatalogue is implemented as a static class and should never be
 * instantiated.
 *
 * \note	StructureCatalogue::init() must be called prior to use.
 */
class StructureCatalogue
{
public:
	StructureCatalogue() = delete;

	static void init(const std::string& filename);

	static const StructureType& getType(StructureID id);

	static Structure* get(StructureID id, Tile* tile = nullptr);

	static const StorableResources& costToBuild(StructureID id);
	static const StorableResources& recyclingValue(StructureID id);

	static bool canBuild(const StorableResources& source, StructureID id);
};
