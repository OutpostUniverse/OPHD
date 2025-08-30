#include "ColonyShip.h"

#include "../Constants/Numbers.h"
#include "../Constants/Strings.h"

#include <NAS2D/ParserHelper.h>

namespace
{
	constexpr int ColonyShipOrbitTime{24};


	ColonyShipLanders readLanders(NAS2D::Xml::XmlElement* element)
	{
		if (!element) { return {}; }

		const auto dictionary = NAS2D::attributesToDictionary(*element);
		return {
			.colonist = dictionary.get<int>("colonist_landers"),
			.cargo = dictionary.get<int>("cargo_landers"),
		};
	}


	int readTurnCount(NAS2D::Xml::XmlElement* element)
	{
		return (element) ? NAS2D::attributesToDictionary(*element).get<int>("count") : 0;
	}
}


ColonyShip colonyShipFromSave(NAS2D::Xml::XmlDocument& xmlDocument)
{
	auto* root = xmlDocument.firstChildElement(constants::SaveGameRootNode);
	if (!root) { throw std::runtime_error("Invalid save game root element."); }

	return {
		readLanders(root->firstChildElement("population")),
		readTurnCount(root->firstChildElement("turns")),
	};
}


int ColonyShip::maxOrbitTime()
{
	return ColonyShipOrbitTime;
}


ColonyShip::ColonyShip() :
	mLanders
	{
		.colonist = 2,
		.cargo = 2,
	},
	mTurnsOfManeuveringFuel{ColonyShipOrbitTime + 1}
{}


ColonyShip::ColonyShip(const ColonyShipLanders& colonyShipLanders, int turnCount) :
	mLanders{colonyShipLanders},
	mTurnsOfManeuveringFuel{(turnCount <= ColonyShipOrbitTime) ? ColonyShipOrbitTime - turnCount + 1 : 0}
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
