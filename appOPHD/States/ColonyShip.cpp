#include "ColonyShip.h"

#include "../Constants/Numbers.h"
#include "../Constants/Strings.h"

#include <NAS2D/ParserHelper.h>

namespace
{
	ColonyShipLanders readLanders(NAS2D::Xml::XmlElement* element)
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

	return {
		readLanders(root->firstChildElement("population")),
		readManeuveringFuel(root->firstChildElement("turns")),
	};
}


ColonyShip::ColonyShip() :
	mLanders
	{
		.colonistLanders = 2,
		.cargoLanders = 2,
	},
	mTurnsOfManeuveringFuel{constants::ColonyShipOrbitTime + 1}
{}


ColonyShip::ColonyShip(const ColonyShipLanders& colonyShipLanders, int turnsOfManeuveringFuel) :
	mLanders{colonyShipLanders},
	mTurnsOfManeuveringFuel{turnsOfManeuveringFuel}
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
