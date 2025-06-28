#pragma once


namespace NAS2D
{
	namespace Xml
	{
		class XmlDocument;
	}
}


struct ColonyShipLanders
{
	int colonistLanders{};
	int cargoLanders{};
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
	const ColonyShipLanders& crashedLanders() const { return mCrashedLanders; }
	void onTurn();

private:
	ColonyShipLanders mLanders;
	ColonyShipLanders mCrashedLanders{};
	int mTurnsOfManeuveringFuel{};
};
