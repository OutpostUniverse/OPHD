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
 * StructureCatalog is implemented as a static class and should never be
 * instantiated.
 *
 * \note	StructureCatalog::init() must be called prior to use.
 */
class StructureCatalog
{
public:
	StructureCatalog() = delete;

	static void init(const std::string& filename);

	static const StructureType& getType(StructureID id);
	static const StructureType& getType(std::size_t index);

	static Structure* create(StructureID id, Tile* tile = nullptr);

	static const StorableResources& costToBuild(StructureID id);
	static const StorableResources& recyclingValue(StructureID id);

	static bool canBuild(StructureID id, const StorableResources& source);
};
