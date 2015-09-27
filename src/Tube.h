#include "Things/Structures/Structure.h"
#include "Common.h"

class Tube : public Structure
{
public:
	Tube(ConnectorDir _cd, bool underground) :	Structure("Tube", "structures/tubes.sprite")
	{
		if (underground)
		{
			if (_cd == CONNECTOR_INTERSECTION)
				sprite().play("ug_intersection");
			else if (_cd == CONNECTOR_RIGHT)
				sprite().play("ug_right");
			else if (_cd == CONNECTOR_LEFT)
				sprite().play("ug_left");
			else
				throw Exception(0, "Bad Connector", "Tried to create a Tube structure with CONNECTOR_NONE paramter.");
		}
		else
		{
			if (_cd == CONNECTOR_INTERSECTION)
				sprite().play("ag_intersection");
			else if (_cd == CONNECTOR_RIGHT)
				sprite().play("ag_right");
			else if (_cd == CONNECTOR_LEFT)
				sprite().play("ag_left");
			else
				throw Exception(0, "Bad Connector", "Tried to create a Tube structure with CONNECTOR_NONE paramter.");
		}

		isConnector(true);

		connectorDirection(_cd);

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
