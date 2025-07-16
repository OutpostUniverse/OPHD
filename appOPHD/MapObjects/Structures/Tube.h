#pragma once

#include "../Structure.h"


class Tile;


class Tube : public Structure
{
public:
	Tube(Tile& tile, ConnectorDir dir);

private:
	static const std::string& getAnimationName(ConnectorDir dir, bool underground);
};
