#pragma once

#include "../Robot.h"

#include "../../Constants/Strings.h"
#include "../../Map/Tile.h"


class Robodozer : public Robot
{
public:
	Robodozer(): Robot(constants::Robodozer, "robots/robodozer.sprite", Robot::Type::Dozer)
	{
	}

	void startTask(Tile& tile) override
	{
		Robot::startTask(tile);

		tileIndex(static_cast<std::size_t>(tile.index()));
		tile.index(TerrainType::Dozed);
	}

	void abortTask(Tile& tile)
	{
		tile.index(static_cast<TerrainType>(mTileIndex));
	}

	void tileIndex(std::size_t index) { mTileIndex = index; }
	std::size_t tileIndex() const { return mTileIndex; }

private:
	std::size_t mTileIndex = 0;
};
