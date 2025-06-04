#pragma once

#include "../Constants/Numbers.h"

#include <optional>


namespace NAS2D
{
	namespace Xml
	{
		class XmlDocument;
	}
}


struct ColonyShipData
{
	int colonistLanders = 0;
	int cargoLanders = 0;
	int turnsOfManeuveringFuel = 0;
};


ColonyShipData colonyShipDataFromSave(NAS2D::Xml::XmlDocument&);


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
	void onTurn();

	const std::optional<ColonyShipData>& crashData() const { return mCrashData; }
private:
	ColonyShipData mColonyShipData = 
	{
		.colonistLanders = 2,
		.cargoLanders = 2,
		.turnsOfManeuveringFuel = constants::ColonyShipOrbitTime + 1
	};
	std::optional<ColonyShipData> mCrashData = std::nullopt;
};
