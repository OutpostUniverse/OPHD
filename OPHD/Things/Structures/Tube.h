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

		setAnimationState();
	}

private:

	void setAnimationState()
	{
		if (mUnderground)
		{
			if (connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION)
			{
				sprite().play(constants::UG_TUBE_INTERSECTION);
			}
			else if (connectorDirection() == ConnectorDir::CONNECTOR_RIGHT)
			{
				sprite().play(constants::UG_TUBE_RIGHT);
			}
			else if (connectorDirection() == ConnectorDir::CONNECTOR_LEFT)
			{
				sprite().play(constants::UG_TUBE_LEFT);
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
				sprite().play(constants::AG_TUBE_INTERSECTION);
			}
			else if (connectorDirection() == ConnectorDir::CONNECTOR_RIGHT)
			{
				sprite().play(constants::AG_TUBE_RIGHT);
			}
			else if (connectorDirection() == ConnectorDir::CONNECTOR_LEFT)
			{
				sprite().play(constants::AG_TUBE_LEFT);
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
