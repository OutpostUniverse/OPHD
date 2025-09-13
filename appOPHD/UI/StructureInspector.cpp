#include "StructureInspector.h"

#include "../CacheImage.h"
#include "../Constants/UiConstants.h"
#include "../MapObjects/Structure.h"
#include "../MapObjects/Structures/FoodProduction.h"
#include "../MapObjects/Structures/OreRefining.h"
#include "../MapObjects/Structures/ResearchFacility.h"
#include "../MapObjects/Structures/Residence.h"
#include "../Resources.h"
#include "StringTable.h"
#include "TextRender.h"

#include <libOPHD/EnumDisabledReason.h>
#include <libOPHD/EnumIdleReason.h>
#include <libOPHD/EnumStructureID.h>

#include <algorithm>
#include <stdexcept>


namespace
{
	constexpr auto windowSizeMin = NAS2D::Vector{390, 250};


	const std::map<DisabledReason, std::string> disabledReasonTable =
	{
		{DisabledReason::None, "Not Disabled"},

		{DisabledReason::Chap, "CHAP Facility unavailable"},
		{DisabledReason::Disconnected, "Not connected to a Command Center"},
		{DisabledReason::Energy, "Insufficient Energy"},
		{DisabledReason::Population, "Insufficient Population"},
		{DisabledReason::RefinedResources, "Insufficient refined resources"},
		{DisabledReason::StructuralIntegrity, "Structural Integrity is compromised"}
	};

	const std::map<IdleReason, std::string> idleReadonTable =
	{
		{IdleReason::None, "Not Idle"},

		{IdleReason::PlayerSet, "Manually set to Idle"},
		{IdleReason::InternalStorageFull, "Internal storage pool full"},
		{IdleReason::FactoryProductionComplete, "Production complete, awaiting shipment"},
		{IdleReason::FactoryInsufficientResources, "Insufficient resources"},
		{IdleReason::FactoryInsufficientRobotCommandCapacity, "Lack of robot command capacity"},
		{IdleReason::FactoryInsufficientWarehouseSpace, "Lack of Warehouse space"},
		{IdleReason::MineExhausted, "Mine exhausted"},
		{IdleReason::MineInactive, "Mine inactive"},
		{IdleReason::InsufficientLuxuryProduct, "Insufficient Luxury Product"}
	};


	std::string getDisabledReason(const Structure& structure)
	{
		return (structure.disabled()) ?
			disabledReasonTable.at(structure.disabledReason()) :
			(structure.isIdle()) ?
				idleReadonTable.at(structure.idleReason()) :
				"";
	}


	std::string formatAge(const Structure& structure)
	{
		return structure.ages() ? std::to_string(structure.age()) + " of " + std::to_string(structure.maxAge()) : "N/A";
	}


	StringTable buildGenericStructureAttributesStringTable(const Structure& structure)
	{
		StringTable stringTable{2, 9};

		stringTable[{0, 0}].text = "Type:";
		stringTable[{1, 0}].text = structure.classDescription();

		stringTable[{0, 1}].text = "State:";
		stringTable[{1, 1}].text = structure.stateDescription(structure.state());

		stringTable[{1, 2}].text = getDisabledReason(structure);

		if (structure.underConstruction())
		{
			stringTable[{0, 3}].text = "Turns Remaining:";
			stringTable[{1, 3}].text = std::to_string(structure.turnsToBuild() - structure.age());
		}
		else
		{
			stringTable[{0, 3}].text = "Age:";
			stringTable[{1, 3}].text = formatAge(structure);
		}

		if (!structure.underConstruction() && !structure.destroyed())
		{
			stringTable[{0, 4}].text = "Integrity:";
			stringTable[{1, 4}].text = std::to_string(structure.integrity());
		}

		stringTable[{0, 5}].text = "Power Required:";
		stringTable[{1, 5}].text = std::to_string(structure.energyRequirement());

		const auto& populationAvailable = structure.populationAvailable();
		const auto& populationRequirements = structure.populationRequirements();

		if (populationRequirements.workers > 0)
		{
			stringTable[{0, 6}].text = "Workers:";
			stringTable[{1, 6}].text = std::to_string(populationAvailable.workers) + " / " + std::to_string(populationRequirements.workers);
			stringTable[{1, 6}].textColor = populationAvailable.workers >= populationRequirements.workers ? NAS2D::Color::White : NAS2D::Color::Red;
		}

		if (populationRequirements.scientists > 0)
		{
			stringTable[{0, 7}].text = "Scientists:";
			stringTable[{1, 7}].text = std::to_string(populationAvailable.scientists) + " / " + std::to_string(populationRequirements.scientists);
			stringTable[{1, 7}].textColor = populationAvailable.scientists >= populationRequirements.scientists ? NAS2D::Color::White : NAS2D::Color::Red;
		}

		if (structure.hasCrime())
		{
			stringTable[{0, 8}].text = "Crime Rate:";
			stringTable[{1, 8}].text = std::to_string(structure.crimeRate()) + "%";
		}

		return stringTable;
	}


	StringTable structureStringTable(const Structure& structure)
	{
		if (structure.energyProducedMax() > 0)
		{
			StringTable stringTable(2, 1);

			stringTable[{0, 0}].text = "Power Produced:";

			auto produced = structure.energyProduced();

			stringTable[{1, 0}].text = std::to_string(produced) + " / " + std::to_string(structure.energyProducedMax());

			if (produced == 0)
			{
				stringTable[{1, 0}].textColor = constants::WarningTextColor;
			}

			return stringTable;
		}

		return StringTable(0, 0);
	}


	StringTable commTowerStringTable(const Structure& structure)
	{
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Communication Range:";

		auto communicationRange = structure.commRange();
		stringTable[{1, 0}].text = std::to_string(communicationRange);

		if (communicationRange == 0)
		{
			stringTable[{1, 0}].textColor = constants::WarningTextColor;
		}

		return stringTable;
	}


	StringTable foodProductionStringTable(const FoodProduction& foodProduction)
	{
		StringTable stringTable(2, 2);

		stringTable[{0, 0}].text = "Food Stored:";
		stringTable[{1, 0}].text = std::to_string(foodProduction.foodLevel()) + " / " + std::to_string(foodProduction.foodStorageCapacity());

		stringTable[{0, 1}].text = "Production Rate:";
		stringTable[{1, 1}].text = std::to_string(foodProduction.foodProduced());

		return stringTable;
	}


	StringTable oreRefiningStringTable(const OreRefining& oreRefining)
	{
		StringTable stringTable(3, 5);

		stringTable.setColumnFont(0, stringTable.GetDefaultFont());
		stringTable.setRowFont(0, stringTable.GetDefaultTitleFont());
		stringTable.setHorizontalPadding(20);
		stringTable.setColumnJustification(1, StringTable::Justification::Center);
		stringTable.setColumnJustification(2, StringTable::Justification::Center);

		stringTable.setColumnText(
			0,
			{
				"",
				ResourceNamesRefined[0],
				ResourceNamesRefined[1],
				ResourceNamesRefined[2],
				ResourceNamesRefined[3],
			});

		stringTable.setRowText(
			0,
			{
				"Material",
				"Storage",
				"Ore Conversion Rate"
			});

		auto& resources = oreRefining.storage().resources;

		const auto capacity = oreRefining.refinedOreStorageCapacity();
		const auto formatStorageAmount = [capacity](int storageAmount) -> std::string
		{
			return std::to_string(storageAmount) + " / " + std::to_string(capacity);
		};

		stringTable[{1, 1}].text = formatStorageAmount(resources[0]);
		stringTable[{2, 1}].text = std::to_string(oreRefining.oreConversionDivisor(0)) + " : 1";

		stringTable[{1, 2}].text = formatStorageAmount(resources[1]);
		stringTable[{2, 2}].text = std::to_string(oreRefining.oreConversionDivisor(1)) + " : 1";

		stringTable[{1, 3}].text = formatStorageAmount(resources[2]);
		stringTable[{2, 3}].text = std::to_string(oreRefining.oreConversionDivisor(2)) + " : 1";

		stringTable[{1, 4}].text = formatStorageAmount(resources[3]);
		stringTable[{2, 4}].text = std::to_string(oreRefining.oreConversionDivisor(3)) + " : 1";

		return stringTable;
	}


	StringTable recyclingStringTable(const Structure& structure)
	{
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Max Waste Processing Capacity:";
		stringTable[{1, 0}].text = std::to_string(structure.bioWasteProcessingCapacity());

		if (!structure.operational()) {
			stringTable[{1, 0}].textColor = constants::WarningTextColor;
		}

		return stringTable;
	}


	StringTable researchFacilityStringTable(const ResearchFacility& researchFacility)
	{
		StringTable stringTable(2, 3);

		stringTable[{0, 0}].text = "Research Produced:";
		stringTable[{0, 1}].text = "Regular";
		stringTable[{0, 2}].text = "Hot";

		stringTable[{1, 1}].text = std::to_string(researchFacility.regularResearchProduced());
		stringTable[{1, 2}].text = std::to_string(researchFacility.hotResearchProduced());

		return stringTable;
	}


	StringTable residenceStringTable(const Residence& residence)
	{
		StringTable stringTable(2, 6);

		stringTable[{0, 0}].text = "Colonist Capacity:";
		stringTable[{1, 0}].text = std::to_string(residence.residentialCapacity());

		stringTable[{0, 1}].text = "Colonists Assigned:";
		stringTable[{1, 1}].text = std::to_string(residence.assignedColonists());

		stringTable[{0, 3}].text = "Waste Capacity:";
		stringTable[{1, 3}].text = std::to_string(residence.bioWasteStorageCapacity());

		stringTable[{0, 4}].text = "Waste Accumulated:";
		stringTable[{1, 4}].text = std::to_string(residence.wasteAccumulated());

		stringTable[{0, 5}].text = "Waste Overflow:";
		stringTable[{1, 5}].text = std::to_string(residence.wasteOverflow());

		return stringTable;
	}


	StringTable storageTanksStringTable(const Structure& structure)
	{
		StringTable stringTable(2, 5);

		stringTable.setColumnText(
			0,
			{
				"Storage Capacity",
				ResourceNamesRefined[0],
				ResourceNamesRefined[1],
				ResourceNamesRefined[2],
				ResourceNamesRefined[3],
			});

		const auto& storage = structure.storage();
		stringTable.setColumnText(
			1,
			{
				std::to_string(storage.total()) + " / " + std::to_string(structure.refinedOreStorageCapacity() * 4),
				std::to_string(storage.resources[0]),
				std::to_string(storage.resources[1]),
				std::to_string(storage.resources[2]),
				std::to_string(storage.resources[3]),
			});

		return stringTable;
	}


	StringTable buildingSpecificStringTable(const Structure& structure)
	{
		const auto structureId = structure.structureId();

		if (structureId == StructureID::CommTower) { return commTowerStringTable(structure); }
		if (const auto* foodProduction = dynamic_cast<const FoodProduction*>(&structure)) { return foodProductionStringTable(*foodProduction); }
		if (const auto* oreRefining = dynamic_cast<const OreRefining*>(&structure)) { return oreRefiningStringTable(*oreRefining); }
		if (structureId == StructureID::Recycling) { return recyclingStringTable(structure); }
		if (const auto* researchFacility = dynamic_cast<const ResearchFacility*>(&structure)) { return researchFacilityStringTable(*researchFacility); }
		if (structureId == StructureID::Residence) { return residenceStringTable(dynamic_cast<const Residence&>(structure)); }
		if (structureId == StructureID::StorageTanks) { return storageTanksStringTable(structure); }

		return structureStringTable(structure);
	}
}


StructureInspector::StructureInspector() :
	Window{"Structure Details"},
	btnClose{"Close", {50, 20}, {this, &StructureInspector::onClose}},
	mIcons{getImage("ui/icons.png")}
{
	size(windowSizeMin);
	add(btnClose, area().size - btnClose.size() - NAS2D::Vector{constants::Margin, constants::Margin});
}


void StructureInspector::showStructure(const Structure& structure)
{
	mStructure = &structure;
	title(mStructure->name());

	const auto genericStructureAttributes = buildGenericStringTable();
	const auto specificAttributeTablePosition = genericStructureAttributes.area().crossYPoint() + NAS2D::Vector{0, 25};
	const auto specificStructureAttributes = buildSpecificStringTable(specificAttributeTablePosition);

	auto windowSize = NAS2D::Vector{
		std::max({windowSizeMin.x, genericStructureAttributes.area().size.x, specificStructureAttributes.area().size.x}),
		std::max({windowSizeMin.y, specificStructureAttributes.area().endPoint().y - genericStructureAttributes.area().position.y + btnClose.size().y})
	} + NAS2D::Vector{constants::Margin, constants::Margin} * 2;

	size(windowSize);

	btnClose.position(area().endPoint() - btnClose.size() - NAS2D::Vector{constants::Margin, constants::Margin});

	show();
}


void StructureInspector::hideStructure(const Structure& structure)
{
	if (mStructure == &structure)
	{
		hide();
		mStructure = nullptr;
	}
}


void StructureInspector::onVisibilityChange(bool visible)
{
	Window::onVisibilityChange(visible);

	if (visible && !mStructure)
	{
		throw std::runtime_error("StructureInspector needs a valid structure");
	}
}


void StructureInspector::onClose()
{
	visible(false);
}


StringTable StructureInspector::buildGenericStringTable() const
{
	auto stringTable = buildGenericStructureAttributesStringTable(*mStructure);
	stringTable.position(mRect.position + NAS2D::Vector{constants::Margin, sWindowTitleBarHeight + constants::Margin});
	stringTable.setVerticalPadding(5);
	stringTable.computeRelativeCellPositions();
	return stringTable;
}


StringTable StructureInspector::buildSpecificStringTable(NAS2D::Point<int> position) const
{
	auto stringTable = buildingSpecificStringTable(*mStructure);
	stringTable.computeRelativeCellPositions();
	stringTable.position(position);
	return stringTable;
}


void StructureInspector::drawClientArea(NAS2D::Renderer& renderer) const
{
	const auto genericStructureAttributes = buildGenericStringTable();
	const auto specificAttributeTablePosition = genericStructureAttributes.area().crossYPoint() + NAS2D::Vector{0, 20 + constants::Margin};
	const auto specificStructureAttributes = buildSpecificStringTable(specificAttributeTablePosition);

	genericStructureAttributes.draw(renderer);
	specificStructureAttributes.draw(renderer);
}
