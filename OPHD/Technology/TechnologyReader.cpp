#include "TechnologyReader.h"
#include "../XmlSerializer.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/ParserHelper.h>

#include <stdexcept>


using namespace NAS2D;


TechnologyReader::TechnologyReader(const std::string& techFile)
{
	Xml::XmlDocument xmlDocument = openXmlFile(techFile, "technology");

	auto root = xmlDocument.firstChildElement("technology");

	auto firstCategory = root->firstChildElement("category");
	if (!firstCategory) { return; }
	readCategories(*firstCategory);
}


void TechnologyReader::readCategories(NAS2D::Xml::XmlElement& node)
{
	for (auto category = node.firstChildElement(); category; category = node.nextSiblingElement())
	{
		const auto attributes = NAS2D::attributesToDictionary(*category);
		const std::string name = attributes.get<std::string>("name");

		auto it = std::find(mCategories.begin(), mCategories.end(), name);
		if (it != mCategories.end())
		{
			throw std::runtime_error("TechnologyReader: Category redefinition '" + name +
				"' at (" + std::to_string(category->row()) + ", " + std::to_string(category->column()) + ")");
		}

		mCategories.push_back(name);
		readTechnologiesInCategory(*category);
	}
}


void TechnologyReader::readTechnologiesInCategory(NAS2D::Xml::XmlElement& category)
{
	for (auto technology = category.firstChildElement(); technology; technology = category.nextSiblingElement())
	{
		const auto attributes = NAS2D::attributesToDictionary(*technology);

		const int id = attributes.get<int>("id");

		const auto it = std::find_if(mTechnologies.begin(), mTechnologies.end(), [id](const Technology& tech) { return tech.id == id; });
		if (it != mTechnologies.end())
		{
			throw std::runtime_error("TechnologyReader: Technology ID redefinition '" + std::to_string(id) +
				"' at (" + std::to_string(technology->row()) + ", " + std::to_string(technology->column()) + ")");
		}

		readTechnology(*technology, attributes);
	}
}


void TechnologyReader::readTechnology(NAS2D::Xml::XmlElement& technology, const NAS2D::Dictionary& attributes)
{
	Technology tech(attributes.get<int>("id"),
		attributes.get<int>("lab_type"),
		attributes.get<int>("cost"));
	
	for (auto techElement = technology.firstChildElement(); techElement; techElement = technology.nextSiblingElement())
	{
		const std::string elementName = techElement->value();
		std::string elementValue = techElement->getText();
		if (elementName == "name")
		{
			tech.name = elementValue;
		}
		else if (elementName == "description")
		{
			tech.description = elementValue;
		}
		else if (elementName == "requires")
		{
			auto requiredIds = NAS2D::split(elementValue);
			for (auto& id : requiredIds)
			{
				tech.requiredTechnologies.push_back(std::stoi(id));
			}
		}
	}
}
