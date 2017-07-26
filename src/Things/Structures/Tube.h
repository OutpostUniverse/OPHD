#pragma once

#include "Structure.h"

#include "../../Common.h"

class Tube : public Structure
{
public:
	Tube(ConnectorDir _cd, bool underground) :	Structure(constants::TUBE, "structures/tubes.sprite", CLASS_TUBE),
												mUnderground(underground)
	{
		connectorDirection(_cd);
		requiresCHAP(false);

		maxAge(400);

		setAnimationState();
	}


	virtual ~Tube()
	{}

protected:
private:

	void setAnimationState()
	{
		if (mUnderground)
		{
			if (connectorDirection() == CONNECTOR_INTERSECTION)
				sprite().play(constants::UG_TUBE_INTERSECTION);
			else if (connectorDirection() == CONNECTOR_RIGHT)
				sprite().play(constants::UG_TUBE_RIGHT);
			else if (connectorDirection() == CONNECTOR_LEFT)
				sprite().play(constants::UG_TUBE_LEFT);
			else
				throw std::runtime_error("Tried to create a Tube structure with CONNECTOR_NONE paramter.");
		}
		else
		{
			if (connectorDirection() == CONNECTOR_INTERSECTION)
				sprite().play(constants::AG_TUBE_INTERSECTION);
			else if (connectorDirection() == CONNECTOR_RIGHT)
				sprite().play(constants::AG_TUBE_RIGHT);
			else if (connectorDirection() == CONNECTOR_LEFT)
				sprite().play(constants::AG_TUBE_LEFT);
			else
				throw std::runtime_error("Tried to create a Tube structure with CONNECTOR_NONE paramter.");
		}
	}

	bool	mUnderground;
};
