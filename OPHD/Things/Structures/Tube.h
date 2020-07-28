#pragma once

#include "Structure.h"

#include "../../Common.h"

/**
 * Implements a Tube Structure.
 */
class Tube : public Structure
{
public:
	Tube(ConnectorDir dir, bool underground) :
		Structure(constants::TUBE, "structures/tubes.sprite", StructureClass::Tube)
	{
		connectorDirection(dir);
		requiresCHAP(false);

		maxAge(400);

		sprite().play(getAnimationName(dir, underground));
	}

private:

	static const std::string& getAnimationName(ConnectorDir dir, bool underground)
	{
		if (underground)
		{
			if (dir == ConnectorDir::CONNECTOR_INTERSECTION)
			{
				return constants::UG_TUBE_INTERSECTION;
			}
			else if (dir == ConnectorDir::CONNECTOR_RIGHT)
			{
				return constants::UG_TUBE_RIGHT;
			}
			else if (dir == ConnectorDir::CONNECTOR_LEFT)
			{
				return constants::UG_TUBE_LEFT;
			}
			else
			{
				throw std::runtime_error("Tried to create a Tube structure with invalid connector direction paramter.");
			}
		}
		else
		{
			if (dir == ConnectorDir::CONNECTOR_INTERSECTION)
			{
				return constants::AG_TUBE_INTERSECTION;
			}
			else if (dir == ConnectorDir::CONNECTOR_RIGHT)
			{
				return constants::AG_TUBE_RIGHT;
			}
			else if (dir == ConnectorDir::CONNECTOR_LEFT)
			{
				return constants::AG_TUBE_LEFT;
			}
			else
			{
				throw std::runtime_error("Tried to create a Tube structure with invalid connector direction paramter.");
			}
		}
	}
};
