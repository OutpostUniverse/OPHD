#include "PlanetAttributes.h"

#include "XmlSerializer.h"

#include <NAS2D/Dictionary.h>
#include <NAS2D/ParserHelper.h>
#include <NAS2D/Xml/XmlDocument.h>
#include <NAS2D/Xml/XmlElement.h>

#include <unordered_map>


namespace
{
	const std::unordered_map<std::string, PlanetType> planetTypeTable
	{
		{"Mercury", PlanetType::Mercury},
		{"Mars", PlanetType::Mars},
		{"Ganymede", PlanetType::Ganymede}
	};

	const std::unordered_map<std::string, PlanetHostility> hostilityTable
	{
		{"Low", PlanetHostility::Low},
		{"Medium", PlanetHostility::Medium},
		{"High", PlanetHostility::High}
	};


	PlanetAttributes parsePlanet(const NAS2D::Xml::XmlElement* xmlNode)
	{
		NAS2D::Dictionary dictionary{};
		for (const auto* element = xmlNode->firstChildElement(); element; element = element->nextSiblingElement())
		{
			dictionary.set(element->value(), element->getText());
		}

		const auto requiredFields = std::vector<std::string>{"PlanetType", "ImagePath", "Hostility", "MaxDepth", "MaxMines", "MapImagePath", "TilesetPath", "Name", "MeanSolarDistance", "Description"};
		NAS2D::reportMissingOrUnexpected(dictionary.keys(), requiredFields, {});

		return {
			stringToEnum(planetTypeTable, dictionary.get("PlanetType")),
			dictionary.get("ImagePath"),
			stringToEnum(hostilityTable, dictionary.get("Hostility")),
			dictionary.get<int>("MaxDepth"),
			dictionary.get<std::size_t>("MaxMines"),
			dictionary.get("MapImagePath"),
			dictionary.get("TilesetPath"),
			dictionary.get("Name"),
			dictionary.get<float>("MeanSolarDistance"),
			dictionary.get("Description"),
		};
	}
}


std::vector<PlanetAttributes> parsePlanetAttributes(const std::string& filename)
{
	const std::string rootElementName("Planets");
	auto xmlDocument = openXmlFile(filename, rootElementName);

	std::vector<PlanetAttributes> planetAttributes;

	auto rootElement = xmlDocument.firstChildElement(rootElementName);
	for (const auto* node = rootElement->firstChildElement("Planet"); node; node = node->nextSiblingElement("Planet"))
	{
		planetAttributes.push_back(parsePlanet(node));
	}

	return planetAttributes;
}
