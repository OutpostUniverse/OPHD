#include "SeedLander.h"

#include "OPHD/Map/TileMap.h"
#include "OPHD/Map/Tile.h"
#include "OPHD/DirectionOffset.h"

SeedLander::SeedLander(Tile& tile, TileMap& tileMap) :
	DeployableStructure(StructureClass::Lander, StructureID::SID_SEED_LANDER, tile),
	mTileMap(tileMap)
{
}
