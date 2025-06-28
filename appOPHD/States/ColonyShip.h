#pragma once

#include <optional>


namespace NAS2D
{
	namespace Xml
	{
		class XmlDocument;
	}
}


struct ColonyShipLanders
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
	ColonyShip(const ColonyShipLanders&, int turnsOfManeuveringFuel);

	int colonistLanders() const { return mColonyShipData.colonistLanders; }
	int cargoLanders() const { return mColonyShipData.cargoLanders; }
	int turnsOfManeuveringFuel() const { return mTurnsOfManeuveringFuel; }
	void onDeployColonistLander() { --mColonyShipData.colonistLanders; }
	void onDeployCargoLander() { --mColonyShipData.cargoLanders; }
	bool crashed() const { return mTurnsOfManeuveringFuel == 0; }
	void onTurn();

	const std::optional<ColonyShipLanders>& crashData() const { return mCrashData; }
private:
	ColonyShipLanders mColonyShipData;
	std::optional<ColonyShipLanders> mCrashData = std::nullopt;
	int mTurnsOfManeuveringFuel = 0;
};
