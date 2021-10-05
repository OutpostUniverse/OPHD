#include "Planet.h"

#include "../Constants.h"
#include "../XmlSerializer.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Dictionary.h>
#include <NAS2D/ParserHelper.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Xml/Xml.h>

#include <stdexcept>


namespace
{
	constexpr auto PlanetRadius = 64;
	constexpr auto PlanetSize = NAS2D::Vector{PlanetRadius * 2, PlanetRadius * 2};
}


Planet::Planet(const Attributes& attributes) :
	mAttributes(attributes),
	mImage(NAS2D::Image(attributes.imagePath))
{
	if (attributes.type == PlanetType::None || attributes.type == PlanetType::Count)
	{
		throw std::runtime_error("Instantiated Planet class with an invalid planet type.");
	}

	NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().connect(this, &Planet::onMouseMove);
}


Planet::~Planet()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().disconnect(this, &Planet::onMouseMove);
}


bool Planet::pointInCircle(NAS2D::Point<int> point)
{
	const auto offset = point - mPosition - PlanetSize / 2;
	constexpr auto radiusSquared = PlanetRadius * PlanetRadius;
	return ((offset.x * offset.x) + (offset.y * offset.y) <= radiusSquared);
}


void Planet::onMouseMove(int x, int y, int /*rX*/, int /*rY*/)
{
	bool inArea = pointInCircle({x, y});
	if (inArea != mMouseInArea)
	{
		mMouseInArea = inArea;
		mMouseInArea ? mMouseEnterSignal() : mMouseExitSignal();
		mTimer.reset();
	}
}


void Planet::update()
{
	if (mMouseInArea && mTimer.accumulator() >= constants::PlanetAnimationSpeed)
	{
		mTimer.reset(); // don't care about frame skips.
		++mTick;
	}

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto spriteFrameOffset = NAS2D::Point{mTick % 8 * PlanetSize.x, ((mTick % 64) / 8) * PlanetSize.y};
	renderer.drawSubImage(mImage, mPosition, NAS2D::Rectangle<int>::Create(spriteFrameOffset, PlanetSize));
}

namespace
{
	Planet::Attributes parsePlanet(const NAS2D::Xml::XmlElement* xmlNode);

	const std::unordered_map<std::string, Planet::PlanetType> planetTypeTable
	{
		{"None", Planet::PlanetType::None},
		{"Mercury", Planet::PlanetType::Mercury},
		{"Mars", Planet::PlanetType::Mars},
		{"Ganymede", Planet::PlanetType::Ganymede}
	};

	const std::unordered_map<std::string, Planet::Hostility> hostilityTable
	{
		{"None", Planet::Hostility::None},
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

		Planet::Attributes attributes;
		attributes.type = stringToEnum(planetTypeTable, dictionary.get("PlanetType"));
		attributes.hostility = stringToEnum(hostilityTable, dictionary.get("Hostility"));
		attributes.imagePath = dictionary.get("ImagePath");
		attributes.maxDepth = dictionary.get<int>("MaxDepth");
		attributes.maxMines = dictionary.get<int>("MaxMines");
		attributes.mapImagePath = dictionary.get("MapImagePath");
		attributes.tilesetPath = dictionary.get("TilesetPath");
		attributes.name = dictionary.get("Name");
		attributes.meanSolarDistance = dictionary.get<float>("MeanSolarDistance");
		attributes.description = dictionary.get("Description");

		return attributes;
	}
}
