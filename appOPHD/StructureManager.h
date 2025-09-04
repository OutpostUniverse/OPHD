#pragma once

#include "MapObjects/StructureClass.h"

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

	void removeStructure(Structure& structure);
	void removeAllStructures();

	template <typename StructureType>
	std::vector<StructureType*> getStructures() const
	{
		std::vector<StructureType*> output;
		// Filter for instances of the exact type parameter
		for (auto* structure : mDeployedStructures)
		{
			StructureType* derivedStructure = dynamic_cast<StructureType*>(structure);
			if (derivedStructure)
			{
				output.push_back(derivedStructure);
			}
		}
		return output;
	}

	template <typename Predicate>
	std::vector<Structure*> getStructures(Predicate predicate) const
	{
		std::vector<Structure*> output;
		for (auto* structure : mDeployedStructures)
		{
			if (predicate(*structure))
			{
				output.push_back(structure);
			}
		}
		return output;
	}

	StructureList structureList(StructureClass structureClass) const;
	StructureList structureList(StructureID structureId) const;
	const StructureList& allStructures() const;

	StructureList agingStructures() const;
	StructureList newlyBuiltStructures() const;
	StructureList structuresWithCrime() const;

	StructureList commandCenters() const;
	StructureList activeCommandCenters() const;
	StructureList activePoliceStations() const;

	bool hasCommandCenter() const;
	CommandCenter& firstCc() const;
	std::vector<MapCoordinate> operationalCommandCenterPositions() const;
	bool isInCcRange(NAS2D::Point<int> position) const;
	bool isInCommRange(NAS2D::Point<int> position) const;

	void updateConnectedness(TileMap& tileMap);
	std::vector<Tile*> getConnectednessOverlay() const;

	int count() const;

	int operationalCount(StructureClass structureClass) const;
	int operationalCount(StructureID structureId) const;

	int disabledCount() const;
	int destroyedCount() const;

	bool CHAPAvailable() const;

	void updateEnergy();
	int totalEnergyProduction() const { return mTotalEnergyOutput; }
	int totalEnergyUsed() const { return mTotalEnergyUsed; }
	int totalEnergyAvailable() const { return mTotalEnergyOutput - mTotalEnergyUsed; }

	int totalRefinedOreStorageCapacity() const;
	int totalFoodStorageCapacity() const;
	int totalResidentialCapacity() const;
	int totalBioWasteStorageCapacity() const;
	int totalBioWasteProcessingCapacity() const;
	int totalRobotCommandCapacity() const;

	void assignColonistsToResidences(PopulationPool&);
	void assignScientistsToResearchFacilities(PopulationPool&);

	void update(const StorableResources&, PopulationPool&);

	NAS2D::Xml::XmlElement* serialize() const;

protected:
	void addStructure(Structure& structure, Tile& tile);

	void disconnectAll();
	void updateEnergyProduction();
	void updateEnergyConsumed();

	void updateStructures(const StorableResources&, PopulationPool&, StructureList&);
	void updateStructure(const StorableResources&, PopulationPool&, Structure&);

private:
	std::vector<Structure*> mDeployedStructures;

	int mTotalEnergyOutput = 0;
	int mTotalEnergyUsed = 0;
};
