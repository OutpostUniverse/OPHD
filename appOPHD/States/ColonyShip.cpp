#include "ColonyShip.h"

#include "appOPHD/Constants/Strings.h"

ColonyShip::ColonyShip()
{}


ColonyShip::ColonyShip(const ColonyShipData& colonyShipData) :
	mColonyShipData(colonyShipData)
{}


void ColonyShip::onTurn()
{
	if (mColonyShipData.turnsOfManeuveringFuel > 0) { --mColonyShipData.turnsOfManeuveringFuel; }

	if (mColonyShipData.turnsOfManeuveringFuel == 0)
	{
		mCrashData = mColonyShipData;
		mColonyShipData.cargoLanders = 0;
		mColonyShipData.colonistLanders = 0;
	}
}
