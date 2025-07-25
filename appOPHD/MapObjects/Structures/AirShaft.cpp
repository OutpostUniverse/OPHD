#include "AirShaft.h"

#include "../../Constants/Strings.h"
#include "../../Map/Tile.h"

#include <libOPHD/EnumStructureID.h>


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


void AirShaft::forced_state_change(StructureState, DisabledReason, IdleReason)
{
}
