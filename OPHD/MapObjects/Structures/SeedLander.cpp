#include "SeedLander.h"

#include "OPHD/Map/TileMap.h"
#include "OPHD/Map/Tile.h"
#include "OPHD/DirectionOffset.h"
#include "OPHD/StructureManager.h"

#include <NAS2D/Utility.h>

SeedLander::SeedLander(Tile& tile, TileMap& tileMap) :
	DeployableStructure(StructureClass::Lander, StructureID::SID_SEED_LANDER, tile),
	mTileMap(tileMap)
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
