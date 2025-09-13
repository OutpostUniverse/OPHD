#pragma once

#include "../Structure.h"


class Tube : public Structure
{
public:
	Tube(Tile& tile);

	void connectorDirection(ConnectorDir dir) override;
};
