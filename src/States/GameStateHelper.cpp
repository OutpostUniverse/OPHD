#include "GameStateHelper.h"

// Throw away string stream for font rendering.
stringstream str;

void drawString(Renderer& r, Font& f, std::string s, int i, int x, int y, int red, int green, int blue)
{
	str.str("");
	str << s << i;
	r.drawText(f, str.str(), (float)x, (float)y, red, green, blue);
}


void drawNumber(Renderer& r, Font& f, int i, int x, int y, int red, int green, int blue)
{
	str.str("");
	str << i;
	r.drawText(f, str.str(), (float)x, (float)y, red, green, blue);
}


bool checkTubeConnection(Tile* tile, Direction dir, StructureType type)
{
	if (tile->mine() || !tile->bulldozed() || !tile->excavated() || !tile->thingIsStructure())
		return false;

	Structure* _structure = tile->structure();

	if (type == STRUCTURE_TUBE_INTERSECTION)
	{
		if (dir == DIR_EAST || dir == DIR_WEST)
		{
			if (_structure->connectorDirection() == CONNECTOR_INTERSECTION || _structure->connectorDirection() == CONNECTOR_RIGHT || _structure->connectorDirection() == CONNECTOR_VERTICAL)
				return true;
		}
		else // NORTH/SOUTH
		{
			if (_structure->connectorDirection() == CONNECTOR_INTERSECTION || _structure->connectorDirection() == CONNECTOR_LEFT || _structure->connectorDirection() == CONNECTOR_VERTICAL)
				return true;
		}
	}
	else if (type == STRUCTURE_TUBE_RIGHT && (dir == DIR_EAST || dir == DIR_WEST))
	{
		if (_structure->connectorDirection() == CONNECTOR_INTERSECTION || _structure->connectorDirection() == CONNECTOR_RIGHT || _structure->connectorDirection() == CONNECTOR_VERTICAL)
			return true;
	}
	else if (type == STRUCTURE_TUBE_LEFT && (dir == DIR_NORTH || dir == DIR_SOUTH))
	{
		if (_structure->connectorDirection() == CONNECTOR_INTERSECTION || _structure->connectorDirection() == CONNECTOR_LEFT || _structure->connectorDirection() == CONNECTOR_VERTICAL)
			return true;
	}

	return false;
}

