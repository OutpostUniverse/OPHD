#pragma once

#include "../Structure.h"


class Tube : public Structure
{
public:
	Tube(Tile& tile);
	Tube(Tile& tile, ConnectorDir dir);

	void connectorDirection(ConnectorDir dir) override;
};
