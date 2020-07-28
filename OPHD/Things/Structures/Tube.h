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
		Structure(constants::TUBE, "structures/tubes.sprite", StructureClass::Tube),
		mUnderground(underground)
	{
		connectorDirection(dir);
		requiresCHAP(false);

		maxAge(400);

		sprite().play(getAnimationName());
	}

private:

	const std::string& getAnimationName()
	{
		if (mUnderground)
		{
			if (connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION)
			{
				return constants::UG_TUBE_INTERSECTION;
			}
			else if (connectorDirection() == ConnectorDir::CONNECTOR_RIGHT)
			{
				return constants::UG_TUBE_RIGHT;
			}
			else if (connectorDirection() == ConnectorDir::CONNECTOR_LEFT)
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
			if (connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION)
			{
				return constants::AG_TUBE_INTERSECTION;
			}
			else if (connectorDirection() == ConnectorDir::CONNECTOR_RIGHT)
			{
				return constants::AG_TUBE_RIGHT;
			}
			else if (connectorDirection() == ConnectorDir::CONNECTOR_LEFT)
			{
				return constants::AG_TUBE_LEFT;
			}
			else
			{
				throw std::runtime_error("Tried to create a Tube structure with invalid connector direction paramter.");
			}
		}
	}

private:
	bool mUnderground;
};
