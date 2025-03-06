#pragma once

#include "appOPHD/Constants/Numbers.h"
struct ColonyShipData
{
	int colonistLanders = 0;
	int cargoLanders = 0;
	int turnsOfManeuveringFuel = 0;
};

class GameState;

class ColonyShip
{
public:
	ColonyShip();
	ColonyShip(const ColonyShipData&);

	int colonistLanders() const { return mColonyShipData.colonistLanders; }
	int cargoLanders() const { return mColonyShipData.cargoLanders; }
	int turnsOfManeuveringFuel() const { return mColonyShipData.turnsOfManeuveringFuel; }
	void onDeployColonistLander() { --mColonyShipData.colonistLanders; }
	void onDeployCargoLander() { --mColonyShipData.cargoLanders; }
	bool crashed() const { return mColonyShipData.turnsOfManeuveringFuel == 0; }

private:
	ColonyShipData mColonyShipData = 
	{
		.colonistLanders = 2,
		.cargoLanders = 2,
		.turnsOfManeuveringFuel = constants::ColonyShipOrbitTime + 1
	};
};
