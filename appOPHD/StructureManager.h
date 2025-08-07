#pragma once

#include "MapObjects/StructureClass.h"
#include "MapObjects/StructureTypeToClass.h"

#include <map>
#include <vector>


namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}

	template <typename BaseType> struct Point;
}

enum class StructureID;
enum class StructureState;
class Tile;
class TileMap;
class Structure;
class CommandCenter;
class PopulationPool;
struct StorableResources;
struct MapCoordinate;

using StructureList = std::vector<Structure*>;


/**
 * Handles structure updating and resource management for structures.
 *
 * Keeps track of which structures are operational, idle and disabled.
 */
class StructureManager
{
public:
	StructureManager();

	template <typename StructureType>
	StructureType& create(Tile& tile)
	{
		auto& structure = *new StructureType(tile);
		addStructure(structure, tile);
		return structure;
	}

	Structure& create(StructureID structureId, Tile& tile);

	void addStructure(Structure& structure, Tile& tile);
	void removeStructure(Structure& structure);
	void removeAllStructures();

	template <typename StructureType>
	const std::vector<StructureType*> getStructures() const
	{
		// Get list of structures with same function
		const auto& sameClassStructures = structureList(structureTypeToClass<StructureType>());

		std::vector<StructureType*> output;
		// Filter for instances of the exact type parameter
		for (auto* structure : sameClassStructures)
		{
			StructureType* derivedStructure = dynamic_cast<StructureType*>(structure);
			if (derivedStructure)
			{
				output.push_back(derivedStructure);
			}
		}
		return output;
	}

	const StructureList& structureList(StructureClass structureClass) const;
	StructureList allStructures() const;

	StructureList activePoliceStations() const;

	bool hasCommandCenter() const;
	CommandCenter& firstCc() const;
	std::vector<MapCoordinate> operationalCommandCenterPositions() const;
	bool isInCcRange(NAS2D::Point<int> position) const;
	bool isInCommRange(NAS2D::Point<int> position) const;

	void updateConnectedness(TileMap& tileMap);
	std::vector<Tile*> getConnectednessOverlay() const;

	int count() const;

	int getCountInState(StructureClass structureClass, StructureState state) const;

	template <typename StructureType>
	unsigned int countInState(StructureState state) const
	{
		unsigned int count = 0;
		for (const auto* structure : getStructures<StructureType>())
		{
			if (structure->state() == state)
			{
				++count;
			}
		}
		return count;
	}

	const StructureList& agingStructures() const { return mAgingStructures; }
	const StructureList& newlyBuiltStructures() const { return mNewlyBuiltStructures; }
	const StructureList& structuresWithCrime() const { return mStructuresWithCrime; }

	int disabledCount() const;
	int destroyedCount() const;

	bool CHAPAvailable() const;

	void updateEnergyProduction();
	void updateEnergyConsumed();
	int totalEnergyProduction() const { return mTotalEnergyOutput; }
	int totalEnergyUsed() const { return mTotalEnergyUsed; }
	int totalEnergyAvailable() const { return mTotalEnergyOutput - mTotalEnergyUsed; }

	int totalRefinedOreStorageCapacity() const;
	int totalFoodStorageCapacity() const;
	int totalRobotCommandCapacity() const;

	void assignColonistsToResidences(PopulationPool&);
	void assignScientistsToResearchFacilities(PopulationPool&);

	void update(const StorableResources&, PopulationPool&);

	NAS2D::Xml::XmlElement* serialize() const;

protected:
	void disconnectAll();

	void updateStructures(const StorableResources&, PopulationPool&, StructureList&);
	void updateStructure(const StorableResources&, PopulationPool&, Structure&);

private:
	std::vector<Structure*> mDeployedStructures;
	std::map<StructureClass, StructureList> mStructureLists;

	StructureList mAgingStructures;
	StructureList mNewlyBuiltStructures;
	StructureList mStructuresWithCrime;

	int mTotalEnergyOutput = 0; /**< Total energy output of all energy producers in the structure list. */
	int mTotalEnergyUsed = 0;
};
