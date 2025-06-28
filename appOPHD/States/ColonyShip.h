#pragma once

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
};


class ColonyShip;


ColonyShip colonyShipFromSave(NAS2D::Xml::XmlDocument&);


class ColonyShip
{
public:
	ColonyShip();
	ColonyShip(const ColonyShipData&, int turnsOfManeuveringFuel);

	int colonistLanders() const { return mColonyShipData.colonistLanders; }
	int cargoLanders() const { return mColonyShipData.cargoLanders; }
	int turnsOfManeuveringFuel() const { return mTurnsOfManeuveringFuel; }
	void onDeployColonistLander() { --mColonyShipData.colonistLanders; }
	void onDeployCargoLander() { --mColonyShipData.cargoLanders; }
	bool crashed() const { return mTurnsOfManeuveringFuel == 0; }
	void onTurn();

	const std::optional<ColonyShipData>& crashData() const { return mCrashData; }
private:
	ColonyShipData mColonyShipData;
	std::optional<ColonyShipData> mCrashData = std::nullopt;
	int mTurnsOfManeuveringFuel = 0;
};
