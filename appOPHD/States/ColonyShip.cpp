#include "ColonyShip.h"

#include "../Constants/Numbers.h"
#include "../Constants/Strings.h"

#include <NAS2D/ParserHelper.h>

namespace
{
	ColonyShipData readLanders(NAS2D::Xml::XmlElement* element)
	{
		if (!element) { return {}; }

		const auto dictionary = NAS2D::attributesToDictionary(*element);
		return {
			.colonistLanders = dictionary.get<int>("colonist_landers"),
			.cargoLanders = dictionary.get<int>("cargo_landers"),
		};
	}


	int readManeuveringFuel(NAS2D::Xml::XmlElement* element)
	{
		const auto turnCount = (element) ? NAS2D::attributesToDictionary(*element).get<int>("count") : 0;
		const auto maneuveringFuel = (turnCount <= constants::ColonyShipOrbitTime) ? constants::ColonyShipOrbitTime - turnCount + 1 : 0;
		return maneuveringFuel;
	}
}


ColonyShip colonyShipFromSave(NAS2D::Xml::XmlDocument& xmlDocument)
{
	auto* root = xmlDocument.firstChildElement(constants::SaveGameRootNode);
	if (!root) { throw std::runtime_error("Invalid save game root element."); }

	ColonyShipData colonyShipData = readLanders(root->firstChildElement("population"));
	colonyShipData.turnsOfManeuveringFuel = readManeuveringFuel(root->firstChildElement("turns"));
	return ColonyShip{colonyShipData};
}


ColonyShip::ColonyShip() :
	mColonyShipData
	{
		.colonistLanders = 2,
		.cargoLanders = 2,
		.turnsOfManeuveringFuel = constants::ColonyShipOrbitTime + 1
	}
{}


ColonyShip::ColonyShip(const ColonyShipData& colonyShipData) :
	mColonyShipData{colonyShipData}
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
