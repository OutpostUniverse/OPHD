#include "Robodozer.h"

#include "../../Constants/Strings.h"
#include "../../Map/Tile.h"


Robodozer::Robodozer() :
	Robot(constants::Robodozer, "robots/robodozer.sprite", Robot::Type::Dozer)
{
}


void Robodozer::startTask(Tile& tile)
{
	Robot::startTask(tile);

	mTileIndex = static_cast<std::size_t>(tile.index());
	tile.index(TerrainType::Dozed);
}


void Robodozer::abortTask(Tile& tile)
{
	tile.index(static_cast<TerrainType>(mTileIndex));
}


void Robodozer::onTaskComplete(TileMap& /*tileMap*/)
{
}
