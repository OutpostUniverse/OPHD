#include "Things/Structures/Structure.h"
#include "Common.h"

class Tube : public Structure
{
public:
	Tube(ConnectorDir _cd, bool underground) :	Structure(constants::TUBE, "structures/tubes.sprite")
	{
		if (underground)
		{
			if (_cd == CONNECTOR_INTERSECTION)
				sprite().play(constants::UG_TUBE_INTERSECTION);
			else if (_cd == CONNECTOR_RIGHT)
				sprite().play(constants::UG_TUBE_RIGHT);
			else if (_cd == CONNECTOR_LEFT)
				sprite().play(constants::UG_TUBE_LEFT);
			else
				throw Exception(0, "Bad Connector", "Tried to create a Tube structure with CONNECTOR_NONE paramter.");
		}
		else
		{
			if (_cd == CONNECTOR_INTERSECTION)
				sprite().play(constants::AG_TUBE_INTERSECTION);
			else if (_cd == CONNECTOR_RIGHT)
				sprite().play(constants::AG_TUBE_RIGHT);
			else if (_cd == CONNECTOR_LEFT)
				sprite().play(constants::AG_TUBE_LEFT);
			else
				throw Exception(0, "Bad Connector", "Tried to create a Tube structure with CONNECTOR_NONE paramter.");
		}

		isConnector(true);
		connectorDirection(_cd);
		requiresCHAP(false);

		maxAge(400);
	}


	~Tube()
	{
	}

	void update()
	{
	}

protected:
private:

};
