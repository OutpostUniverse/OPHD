#include "Planet.h"

#include "../Constants.h"
#include "../XmlSerializer.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
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

	void parseElementValue(Planet::PlanetType& destination, const NAS2D::Xml::XmlElement* element);
	void parseElementValue(Planet::Hostility& destination, const NAS2D::Xml::XmlElement* element);

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


	void parseElementValue(Planet::PlanetType& destination, const NAS2D::Xml::XmlElement* element)
	{
		destination = stringToEnum(planetTypeTable, element->getText());
	}


	void parseElementValue(Planet::Hostility& destination, const NAS2D::Xml::XmlElement* element)
	{
		destination = stringToEnum(hostilityTable, element->getText());
	}
}


std::vector<Planet::Attributes> parsePlanetAttributes()
{
	const std::string rootElementName("Planets");
	auto xmlDocument = openXmlFile("planets/PlanetAttributes.xml", rootElementName);

	std::vector<Planet::Attributes> planetAttributes;

	auto rootElement = xmlDocument.firstChildElement(rootElementName);
	for (const auto* node = rootElement->iterateChildren(nullptr);
		node != nullptr;
		node = rootElement->iterateChildren(node))
	{
		std::string elementName("Planet");
		if (node->value() != elementName)
		{
			throw std::runtime_error(xmlDocument.value() + " missing " + elementName + " tag");
		}
		planetAttributes.push_back(parsePlanet(node->toElement()));
	}

	return planetAttributes;
}


namespace
{
	Planet::Attributes parsePlanet(const NAS2D::Xml::XmlElement* xmlNode)
	{
		Planet::Attributes attributes;

		for (const auto* node = xmlNode->iterateChildren(nullptr);
			node != nullptr;
			node = xmlNode->iterateChildren(node))
		{
			const auto* element = node->toElement();

			if (element->value() == "PlanetType")
			{
				parseElementValue(attributes.type, element);
			}
			else if (element->value() == "ImagePath")
			{
				::parseElementValue(attributes.imagePath, element);
			}
			else if (element->value() == "Hostility")
			{
				parseElementValue(attributes.hostility, element);
			}
			else if (element->value() == "MaxDepth")
			{
				::parseElementValue(attributes.maxDepth, element);
			}
			else if (element->value() == "MaxMines")
			{
				::parseElementValue(attributes.maxMines, element);
			}
			else if (element->value() == "MapImagePath")
			{
				::parseElementValue(attributes.mapImagePath, element);
			}
			else if (element->value() == "TilesetPath")
			{
				::parseElementValue(attributes.tilesetPath, element);
			}
			else if (element->value() == "Name")
			{
				::parseElementValue(attributes.name, element);
			}
			else if (element->value() == "MeanSolarDistance")
			{
				::parseElementValue(attributes.meanSolarDistance, element);
			}
			else if (element->value() == "Description")
			{
				::parseElementValue(attributes.description, element);
			}
			else
			{
				throw std::runtime_error("Unexpected Xml node named " + xmlNode->value() + " encountered. when parsing planet attributes");
			}
		}

		return attributes;
	}
}
