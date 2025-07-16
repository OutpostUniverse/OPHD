#include "AirShaft.h"


#include "../../Constants/Strings.h"


AirShaft::AirShaft() :
	Structure{
		StructureClass::Tube,
		StructureID::SID_AIR_SHAFT,
		constants::StructureStateOperational,
	}
{
	connectorDirection(ConnectorDir::CONNECTOR_VERTICAL);

	state(StructureState::Operational);
}


void AirShaft::underground()
{
	mSprite.play(constants::StructureStateOperationalUg);
	mIsUnderground = true;
}


void AirShaft::forced_state_change(StructureState, DisabledReason, IdleReason)
{
	if (mIsUnderground)
	{
		return;
	}
}
