#include "AirShaft.h"

#include "../StructureState.h"
#include "../../Map/Tile.h"
#include "../../Constants/Strings.h"

#include <libOPHD/EnumStructureID.h>
#include <libOPHD/EnumConnectorDir.h>


AirShaft::AirShaft(Tile& tile) :
	Structure{
		StructureID::AirShaft,
		tile,
		(tile.depth() == 0) ? constants::StructureStateOperational : constants::StructureStateOperationalUg,
	}
{
	connectorDirection(ConnectorDir::Vertical);

	state(StructureState::Operational);
}


void AirShaft::forcedStateChange(StructureState, DisabledReason, IdleReason)
{
}
