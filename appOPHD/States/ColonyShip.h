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
	int colonist{};
	int cargo{};
};


class ColonyShip;


ColonyShip colonyShipFromSave(NAS2D::Xml::XmlDocument& xmlDocument);


class ColonyShip
{
public:
	ColonyShip();
	ColonyShip(const ColonyShipLanders& colonyShipLanders, int turnCount);

	int colonistLanders() const { return mLanders.colonist; }
	int cargoLanders() const { return mLanders.cargo; }
	int turnsOfManeuveringFuel() const { return mTurnsOfManeuveringFuel; }
	void onDeployColonistLander() { --mLanders.colonist; }
	void onDeployCargoLander() { --mLanders.cargo; }
	bool crashed() const { return mTurnsOfManeuveringFuel == 0; }
	const ColonyShipLanders& crashedLanders() const { return mCrashedLanders; }
	void onTurn();

private:
	ColonyShipLanders mLanders;
	ColonyShipLanders mCrashedLanders{};
	int mTurnsOfManeuveringFuel{};
};
