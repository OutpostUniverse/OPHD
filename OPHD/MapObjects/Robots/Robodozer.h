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

		mTileIndex = static_cast<std::size_t>(tile.index());
		tile.index(TerrainType::Dozed);
	}

	void abortTask(Tile& tile) override
	{
		tile.index(static_cast<TerrainType>(mTileIndex));
	}

private:
	std::size_t mTileIndex = 0;
};
