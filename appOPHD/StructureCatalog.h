#pragma once

#include <string>


enum class StructureID;
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

	static std::size_t count();
	static std::size_t typeIndex(StructureID id);

	static const StructureType& getType(StructureID id);
	static const StructureType& getType(std::size_t structureTypeIndex);

	static Structure* create(StructureID id, Tile& tile);
	static Structure* create(std::size_t structureTypeIndex, Tile& tile);

	static const StorableResources& costToBuild(StructureID id);
	static const StorableResources& costToBuild(std::size_t structureTypeIndex);
	static const StorableResources& recyclingValue(StructureID id);
	static const StorableResources& recyclingValue(std::size_t structureTypeIndex);

	static bool canBuild(StructureID id, const StorableResources& source);
	static bool canBuild(std::size_t structureTypeIndex, const StorableResources& source);
};
