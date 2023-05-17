#pragma once

#include "../Robot.h"

class Tile;


class Robodozer : public Robot
{
public:
	Robodozer();

	void startTask(Tile& tile) override;
	void abortTask(Tile& tile) override;

private:
	std::size_t mTileIndex = 0;
};
