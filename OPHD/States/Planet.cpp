#include "Planet.h"

#include "../Constants/Numbers.h"
#include "../XmlSerializer.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Dictionary.h>
#include <NAS2D/ParserHelper.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Xml/Xml.h>

#include <stdexcept>
#include <unordered_map>


namespace
{
	constexpr auto PlanetRadius = 64;
	constexpr auto PlanetSize = NAS2D::Vector{PlanetRadius * 2, PlanetRadius * 2};
}


Planet::Planet(const Attributes& attributes) :
	mAttributes(attributes),
	mImage(NAS2D::Image(attributes.imagePath))
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().connect({this, &Planet::onMouseMove});
}


Planet::~Planet()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().disconnect({this, &Planet::onMouseMove});
}


bool Planet::pointInCircle(NAS2D::Point<int> point)
{
	const auto offset = point - mPosition - PlanetSize / 2;
	constexpr auto radiusSquared = PlanetRadius * PlanetRadius;
	return ((offset.x * offset.x) + (offset.y * offset.y) <= radiusSquared);
}


void Planet::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	bool inArea = pointInCircle(position);
	if (inArea != mMouseInArea)
	{
		mMouseInArea = inArea;
		mMouseInArea ? mMouseEnterSignal() : mMouseExitSignal();
		mTimer.reset();
	}
}


void Planet::update()
{
	if (mMouseInArea && mTimer.elapsedTicks() >= constants::PlanetAnimationSpeed)
	{
		mTimer.reset(); // don't care about frame skips.
		++mTick;
	}

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto spriteFrameOffset = NAS2D::Point{mTick % 16 * PlanetSize.x, ((mTick % 256) / 16) * PlanetSize.y};
	renderer.drawSubImage(mImage, mPosition, NAS2D::Rectangle{spriteFrameOffset, PlanetSize});
}

namespace
{
	Planet::Attributes parsePlanet(const NAS2D::Xml::XmlElement* xmlNode);

	const std::unordered_map<std::string, Planet::PlanetType> planetTypeTable
	{
		{"Mercury", Planet::PlanetType::Mercury},
		{"Mars", Planet::PlanetType::Mars},
		{"Ganymede", Planet::PlanetType::Ganymede}
	};

	const std::unordered_map<std::string, Planet::Hostility> hostilityTable
	{
		{"Low", Planet::Hostility::Low},
		{"Medium", Planet::Hostility::Medium},
		{"High", Planet::Hostility::High}
	};
}


std::vector<Planet::Attributes> parsePlanetAttributes()
{
	const std::string rootElementName("Planets");
	auto xmlDocument = openXmlFile("planets/PlanetAttributes.xml", rootElementName);

	std::vector<Planet::Attributes> planetAttributes;

	auto rootElement = xmlDocument.firstChildElement(rootElementName);
	for (const auto* node = rootElement->firstChildElement("Planet"); node; node = node->nextSiblingElement("Planet"))
	{
		planetAttributes.push_back(parsePlanet(node));
	}

	return planetAttributes;
}


namespace
{
	Planet::Attributes parsePlanet(const NAS2D::Xml::XmlElement* xmlNode)
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
