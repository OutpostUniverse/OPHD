#include "ColonyShip.h"

#include "../Constants/Strings.h"

#include <NAS2D/ParserHelper.h>

namespace
{
	void setLanders(ColonyShipData& colonyShipData, NAS2D::Xml::XmlElement* element)
	{
		if (element)
		{
			const auto dictionary = NAS2D::attributesToDictionary(*element);
			colonyShipData.colonistLanders = dictionary.get<int>("colonist_landers");
			colonyShipData.cargoLanders = dictionary.get<int>("cargo_landers");
		}
	}


	void setManeuveringFuel(ColonyShipData& colonyShipData, NAS2D::Xml::XmlElement* element)
	{
		if (element)
		{
			auto turnCount = NAS2D::attributesToDictionary(*element).get<int>("count");

			colonyShipData.turnsOfManeuveringFuel = turnCount > constants::ColonyShipOrbitTime ? 0 : constants::ColonyShipOrbitTime - turnCount + 1;
		}
	}
}


ColonyShipData colonyShipDataFromSave(NAS2D::Xml::XmlDocument& xmlDocument)
{
	auto* root = xmlDocument.firstChildElement(constants::SaveGameRootNode);
	if (root)
	{
		ColonyShipData colonyShipData;
		setLanders(colonyShipData, root->firstChildElement("population"));
		setManeuveringFuel(colonyShipData, root->firstChildElement("turns"));
		return colonyShipData;
	}
	throw std::runtime_error("Invalid save game root element.");
}


ColonyShip::ColonyShip()
{}


ColonyShip::ColonyShip(const ColonyShipData& colonyShipData) :
	mColonyShipData(colonyShipData)
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
