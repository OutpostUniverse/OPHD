#include "TechnologyReader.h"
#include "../XmlSerializer.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/ParserHelper.h>

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
};


TechnologyReader::TechnologyReader(const std::string& techFile)
{
	Xml::XmlDocument xmlDocument = openXmlFile(techFile, "technology");

	auto root = xmlDocument.firstChildElement("technology");

	auto firstCategory = root->firstChildElement("category");
	if (!firstCategory) { return; }
	readCategories(*firstCategory);
}


const Technology& TechnologyReader::technologyFromId(int id) const
{
	const auto it = std::find_if(mTechnologies.begin(), mTechnologies.end(), [id](const Technology& tech) { return tech.id == id; });

	if (it == mTechnologies.end())
	{
		throw std::runtime_error("TechnologyReader: Requested technology id '" + std::to_string(id) + "' not found.");
	}

	return (*it);
}


void TechnologyReader::readCategories(NAS2D::Xml::XmlElement& node)
{
	for (auto category = &node; category; category = category->nextSiblingElement())
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
	for (auto technology = category.firstChildElement(); technology; technology = technology->nextSiblingElement())
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
	mTechnologies.push_back({attributes.get<int>("id"),
							 attributes.get<int>("lab_type"),
							 attributes.get<int>("cost")});

	auto& tech = mTechnologies.back();
	
	for (auto techElement = technology.firstChildElement(); techElement; techElement = techElement->nextSiblingElement())
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
		else if (elementName == "effects")
		{
			readEffects(*techElement, tech);
		}
		else
		{
			throw std::runtime_error("TechnologyReader: Unknown element '" + elementName + 
				"' at (" + std::to_string(techElement->row()) + ", " + std::to_string(techElement->column()) + ")");
		}
	}
}


void TechnologyReader::readEffects(NAS2D::Xml::XmlElement& effects, Technology& technology)
{
	for (auto effectElement = effects.firstChildElement(); effectElement; effectElement = effectElement->nextSiblingElement())
	{
		const std::string effectName = effectElement->value();
		const std::string effectValue = effectElement->getText();
		auto effectAttributes = NAS2D::attributesToDictionary(*effectElement);

		if (effectName == "modifier")
		{
			technology.effects.push_back(std::make_unique<Technology::Effect>(
				Technology::Modifier(
					StringToModifier.at(effectAttributes.get<std::string>("type")),
					std::stof(effectValue))
				));
		}
		else if (effectName == "unlock")
		{
			technology.effects.push_back(std::make_unique<Technology::Effect>(
				Technology::Unlock(
					StringToUnlock.at(effectAttributes.get<std::string>("type")),
					effectValue)
				));
		}
		else
		{
			throw std::runtime_error("TechnologyReader: Unknown element '" + effectName +
				"' at (" + std::to_string(effectElement->row()) + ", " + std::to_string(effectElement->column()) + ")");
		}
	}
}
