#include "TechnologyCatalog.h"

#include "Technology.h"

#include "../XmlSerializer.h"

#include <NAS2D/ParserHelper.h>
#include <NAS2D/ContainerUtils.h>
#include <NAS2D/StringUtils.h>
#include <NAS2D/Dictionary.h>
#include <NAS2D/Xml/Xml.h>

#include <algorithm>
#include <stdexcept>


using namespace NAS2D;

namespace
{
	std::map<std::string, Technology::Modifier::Modifies> StringToModifier =
	{
		{"agriculture", Technology::Modifier::Modifies::AgricultureEfficiency},
		{"breakdown", Technology::Modifier::Modifies::BreakdownRate},
		{"education", Technology::Modifier::Modifies::EducationEfficiency},
		{"maintenance_cost", Technology::Modifier::Modifies::MaintenanceCost},
		{"pop_fertility", Technology::Modifier::Modifies::PopulationFertility},
		{"pop_morale", Technology::Modifier::Modifies::PopulationMorale},
		{"pop_mortality", Technology::Modifier::Modifies::PopulationMortality},
		{"recycling", Technology::Modifier::Modifies::RecyclingEfficiency},
		{"smelter", Technology::Modifier::Modifies::SmelterEfficiency},
		{"structure_cost", Technology::Modifier::Modifies::StructureCost},
		{"structure_decay", Technology::Modifier::Modifies::StructureDecay}
	};


	std::map<std::string, Technology::Unlock::Unlocks> StringToUnlock =
	{
		{"disaster_prediction", Technology::Unlock::Unlocks::DisasterPrediction},
		{"robot", Technology::Unlock::Unlocks::Robot},
		{"satellite", Technology::Unlock::Unlocks::Satellite},
		{"structure", Technology::Unlock::Unlocks::Structure},
		{"vehicle", Technology::Unlock::Unlocks::Vehicle}
	};


	std::string nodeAtString(const NAS2D::Xml::XmlNode& node)
	{
		return " at (line " + std::to_string(node.row()) + ", column " + std::to_string(node.column()) + ")";
	}


	void verifySubElementTypes(const NAS2D::Xml::XmlElement& parentElement, const std::vector<std::string>& allowedNames, const std::string& errorMessagePrefix)
	{
		for (auto subElement = parentElement.firstChildElement(); subElement; subElement = subElement->nextSiblingElement())
		{
			const auto& elementName = subElement->value();
			if (std::find(allowedNames.begin(), allowedNames.end(), elementName) == allowedNames.end())
			{
				throw std::runtime_error(errorMessagePrefix + "Unknown element '" + elementName + "'" + nodeAtString(*subElement));
			}
		}
	}


	template <typename UnaryOperation>
	auto readSubElementArray(const NAS2D::Xml::XmlElement& parentElement, const std::string& subElementName, UnaryOperation mapFunction)
	{
		using ResultType = decltype(mapFunction(std::declval<NAS2D::Xml::XmlElement&>()));
		using ElementType = std::remove_cv_t<std::remove_reference_t<ResultType>>;

		std::vector<ElementType> results;
		for (auto subElement = parentElement.firstChildElement(subElementName); subElement; subElement = subElement->nextSiblingElement(subElementName))
		{
			results.push_back(mapFunction(*subElement));
		}
		return results;
	}


	NAS2D::Dictionary readAttributesAndSubValues(NAS2D::Xml::XmlElement& element)
	{
		auto attributes = NAS2D::attributesToDictionary(element);
		for (auto subElement = element.firstChildElement(); subElement; subElement = subElement->nextSiblingElement())
		{
			const auto& elementName = subElement->value();
			const auto& elementValue = subElement->getText();
			if (!elementValue.empty())
			{
				attributes.set(elementName, elementValue);
			}
		}
		return attributes;
	}


	Technology::Modifier elementToModifier(const NAS2D::Xml::XmlElement& element)
	{
		return Technology::Modifier{
			StringToModifier.at(element.attribute("type")),
			std::stof(element.getText())
		};
	}


	Technology::Unlock elementToUnlock(const NAS2D::Xml::XmlElement& element) {
		return Technology::Unlock{
			StringToUnlock.at(element.attribute("type")),
			element.getText()
		};
	}


	Technology readTechnology(NAS2D::Xml::XmlElement& technology)
	{
		const auto dictionary = readAttributesAndSubValues(technology);

		try
		{
			reportMissingOrUnexpected(dictionary.keys(), {"id", "lab_type", "cost", "name", "icon_index"}, {"description", "requires", "effects", "icon_index"});
		}
		catch(std::exception& error)
		{
			throw std::runtime_error("TechnologyReader: " + std::string{error.what()} + nodeAtString(technology));
		}

		const auto stoi = [](const auto& string) { return std::stoi(string); };

		Technology tech = {
			dictionary.get("name"),
			dictionary.get("description", std::string{""}),
			dictionary.get<int>("id"),
			dictionary.get<int>("lab_type"),
			dictionary.get<int>("cost"),
			dictionary.get<int>("icon_index"),
			NAS2D::mapToVector(NAS2D::split(dictionary.get("requires", std::string{""})), stoi),
			{},
			{}
		};

		if (const auto* techElement = technology.firstChildElement("effects"))
		{
			verifySubElementTypes(*techElement, {"modifier", "unlock"}, "TechnologyReader: ");

			tech.modifiers = readSubElementArray(*techElement, "modifier", elementToModifier);
			tech.unlocks = readSubElementArray(*techElement, "unlock", elementToUnlock);
		}

		return tech;
	}


	std::vector<Technology> readTechnologiesInCategory(NAS2D::Xml::XmlElement& category)
	{
		std::vector<Technology> technologies;
		for (auto technologyNode = category.firstChildElement(); technologyNode; technologyNode = technologyNode->nextSiblingElement())
		{
			const auto tech = readTechnology(*technologyNode);

			const auto it = std::find_if(technologies.begin(), technologies.end(), [tech](const Technology& technology) { return technology.id == tech.id; });
			if (it != technologies.end())
			{
				throw std::runtime_error("TechnologyReader: Technology ID redefinition '" + std::to_string(tech.id) + "'" + nodeAtString(*technologyNode));
			}

			technologies.push_back(tech);
		}
		return technologies;
	}


	std::vector<TechnologyCatalog::Category> readCategories(NAS2D::Xml::XmlElement& node)
	{
		std::vector<TechnologyCatalog::Category> categories;
		for (auto category = &node; category; category = category->nextSiblingElement())
		{
			const auto attributes = NAS2D::attributesToDictionary(*category);
			const std::string name = attributes.get("name");

			for (const auto& cat : categories)
			{
				if (cat.name == name)
				{
					throw std::runtime_error("TechnologyReader: Category redefinition '" + name + "'" + nodeAtString(*category));
				}
			}
			
			const TechnologyCatalog::Category newCategory{attributes.get<int>("icon_index"), name, readTechnologiesInCategory(*category)};
			categories.push_back(newCategory);
		}

		return categories;
	}
}


TechnologyCatalog::TechnologyCatalog(const std::string& techFile)
{
	Xml::XmlDocument xmlDocument = openXmlFile(techFile, "technology");

	auto root = xmlDocument.firstChildElement("technology");

	auto firstCategory = root->firstChildElement("category");
	if (!firstCategory) { return; }
	mCategories = readCategories(*firstCategory);
}


const std::vector<std::string> TechnologyCatalog::categoryNames()
{
	std::vector<std::string> categoryNames;
	for (const auto& category : mCategories)
	{
		categoryNames.push_back(category.name);
	}

	return categoryNames;
}


const Technology& TechnologyCatalog::technologyFromId(int id) const
{
	for (const auto& category : mCategories)
	{
		const auto& techList = category.technologies;
		const auto it = std::find_if(techList.begin(), techList.end(), [id](const Technology& tech) { return tech.id == id; });

		if (it != techList.end())
		{
			return (*it);
		}
	}

	throw std::runtime_error("TechnologyReader: Requested technology id '" + std::to_string(id) + "' not found.");
}


const std::vector<Technology>& TechnologyCatalog::technologiesInCategory(const std::string& categoryName) const
{
	for (const auto& category : mCategories)
	{
		if (category.name == categoryName)
		{
			return category.technologies;
		}
	}
	
	throw std::runtime_error("No category named '" + categoryName + "' found in catalog.");
}
