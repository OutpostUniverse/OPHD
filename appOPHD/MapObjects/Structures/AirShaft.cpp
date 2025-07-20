#include "AirShaft.h"

#include "../../Constants/Strings.h"
#include "../../Map/Tile.h"


AirShaft::AirShaft(Tile& tile) :
	Structure{
		StructureID::SID_AIR_SHAFT,
		tile,
		(tile.depth() == 0) ? constants::StructureStateOperational : constants::StructureStateOperationalUg,
	}
{
	connectorDirection(ConnectorDir::CONNECTOR_VERTICAL);

	state(StructureState::Operational);
}


void AirShaft::underground()
{
	mSprite.play(constants::StructureStateOperationalUg);
}


void AirShaft::forced_state_change(StructureState, DisabledReason, IdleReason)
{
}
