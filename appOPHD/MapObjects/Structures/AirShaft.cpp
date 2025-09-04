#include "AirShaft.h"

#include "../StructureState.h"
#include "../../Constants/Strings.h"

#include <libOPHD/EnumStructureID.h>
#include <libOPHD/EnumConnectorDir.h>


AirShaft::AirShaft(Tile& tile) :
	Structure{
		StructureID::AirShaft,
		tile,
		constants::StructureStateOperational,
	}
{
	connectorDirection(ConnectorDir::Vertical);
	mStructureState = StructureState::Operational;
}
