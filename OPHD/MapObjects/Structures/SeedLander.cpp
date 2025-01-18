#include "SeedLander.h"

#include "OPHD/Map/TileMap.h"
#include "OPHD/Map/Tile.h"
#include "OPHD/DirectionOffset.h"
#include "OPHD/StructureCatalogue.h"
#include "OPHD/StructureManager.h"
#include "OPHD/MapObjects/Structure.h"

#include <NAS2D/Utility.h>

SeedLander::SeedLander(StructureBuildData buildData) :
	DeployableStructure(StructureClass::Lander, StructureID::SID_SEED_LANDER, *buildData.tile),
	mTileMap(*buildData.tileMap)
{
}


void SeedLander::bulldoze()
{
	for (const auto& direction : DirectionScan3x3)
	{
		mTileMap.getTile({mTile.xy() + direction, 0}).index(TerrainType::Dozed);
	}
}

void SeedLander::placeTubes()
{
	auto& structureManager = NAS2D::Utility<StructureManager>::get();

	// Place initial tubes
	for (const auto& direction : DirectionClockwise4)
	{
		structureManager.addStructure(*new Tube(ConnectorDir::CONNECTOR_INTERSECTION, false), mTileMap.getTile({mTile.xy() + direction, 0}));
	}
}

void SeedLander::placeInitialStructures()
{
	auto& structureManager = NAS2D::Utility<StructureManager>::get();

	constexpr std::array initialStructures{
		std::tuple{DirectionNorthWest, StructureID::SID_SEED_POWER},
		std::tuple{DirectionNorthEast, StructureID::SID_COMMAND_CENTER},
		std::tuple{DirectionSouthWest, StructureID::SID_SEED_FACTORY},
		std::tuple{DirectionSouthEast, StructureID::SID_SEED_SMELTER},
	};

	std::vector<Structure*> structures;
	for (const auto& [direction, structureId] : initialStructures)
	{
		auto* structure = StructureCatalogue::get(structureId);
		structureManager.addStructure(*structure, mTileMap.getTile({mTile.xy() + direction, 0}));
		structures.push_back(structure);
	}

	ccLocation() = mTile.xy() + DirectionNorthEast;
}
