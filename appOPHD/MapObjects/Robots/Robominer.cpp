#include "Robominer.h"

#include "../../Constants/Strings.h"

#include "../../Map/Tile.h"
#include "../../Map/TileMap.h"
#include "../../StructureManager.h"
#include "../Structures/MineFacility.h"
#include "../Structures/MineShaft.h"

#include <libOPHD/DirectionOffset.h>

#include <NAS2D/Utility.h>


Robominer::Robominer() :
	Robot(RobotTypeIndex::Miner)
{
}


MineFacility& Robominer::buildMine(TileMap& tileMap, const MapCoordinate& position)
{
	auto& structureManager = NAS2D::Utility<StructureManager>::get();

	// Surface structure
	auto& robotTile = tileMap.getTile(position);
	auto& mineFacility = structureManager.create<MineFacility>(robotTile);
	mineFacility.maxDepth(tileMap.maxDepth());

	// Tile immediately underneath facility.
	auto& tileBelow = tileMap.getTile(position.translate(MapOffsetDown));
	structureManager.create<MineShaft>(tileBelow);

	robotTile.bulldoze();
	tileBelow.bulldoze();
	tileBelow.excavated(true);

	die();

	return mineFacility;
}


void Robominer::onTaskComplete(TileMap& /*tileMap*/)
{
}
