#include "Robominer.h"

#include "../../Constants/Strings.h"

#include "../../Map/TileMap.h"
#include "../../StructureManager.h"
#include "../Structures/MineFacility.h"
#include "../Structures/MineShaft.h"

#include <libOPHD/DirectionOffset.h>

#include <NAS2D/Utility.h>


Robominer::Robominer() :
	Robot(constants::Robominer, "robots/robominer.sprite", Robot::Type::Miner)
{
}


MineFacility& Robominer::buildMine(TileMap& tileMap, const MapCoordinate& position)
{
	auto& structureManager = NAS2D::Utility<StructureManager>::get();

	// Surface structure
	auto& robotTile = tileMap.getTile(position);
	auto& mineFacility = *new MineFacility(robotTile.mine());
	mineFacility.maxDepth(tileMap.maxDepth());
	structureManager.addStructure(mineFacility, robotTile);

	// Tile immediately underneath facility.
	auto& tileBelow = tileMap.getTile(position.translate(MapOffsetDown));
	structureManager.addStructure(*new MineShaft(), tileBelow);

	robotTile.index(TerrainType::Dozed);
	tileBelow.index(TerrainType::Dozed);
	tileBelow.excavated(true);

	die();

	return mineFacility;
}
