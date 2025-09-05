#include "ColonyShip.h"


namespace
{
	constexpr int ColonyShipOrbitTime{24};
}


int ColonyShip::maxOrbitTime()
{
	return ColonyShipOrbitTime;
}


ColonyShip::ColonyShip() :
	mLanders
	{
		.colonist = 2,
		.cargo = 2,
	},
	mTurnsOfManeuveringFuel{ColonyShipOrbitTime + 1}
{}


ColonyShip::ColonyShip(const ColonyShipLanders& colonyShipLanders, int turnCount) :
	mLanders{colonyShipLanders},
	mTurnsOfManeuveringFuel{(turnCount <= ColonyShipOrbitTime) ? ColonyShipOrbitTime - turnCount + 1 : 0}
{}


void ColonyShip::onTurn()
{
	if (mTurnsOfManeuveringFuel > 0)
	{
		--mTurnsOfManeuveringFuel;
	}
	else
	{
		mCrashedLanders = mLanders;
		mLanders = {};
	}
}
