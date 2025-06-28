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

	int colonistLanders() const { return mLanders.colonistLanders; }
	int cargoLanders() const { return mLanders.cargoLanders; }
	int turnsOfManeuveringFuel() const { return mTurnsOfManeuveringFuel; }
	void onDeployColonistLander() { --mLanders.colonistLanders; }
	void onDeployCargoLander() { --mLanders.cargoLanders; }
	bool crashed() const { return mTurnsOfManeuveringFuel == 0; }
	void onTurn();

	const std::optional<ColonyShipLanders>& crashData() const { return mCrashData; }
private:
	ColonyShipLanders mLanders;
	std::optional<ColonyShipLanders> mCrashData = std::nullopt;
	int mTurnsOfManeuveringFuel = 0;
};
