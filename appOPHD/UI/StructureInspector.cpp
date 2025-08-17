#include "StructureInspector.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"
#include "../MapObjects/Structure.h"
#include "StringTable.h"
#include "TextRender.h"

#include <libOPHD/EnumDisabledReason.h>
#include <libOPHD/EnumIdleReason.h>

#include <algorithm>
#include <stdexcept>


namespace
{
	constexpr auto windowSizeMin = NAS2D::Vector{350, 250};


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
		{IdleReason::FactoryProductionComplete, "Production complete, waiting on product pull."},
		{IdleReason::FactoryInsufficientResources, "Insufficient resources to continue production"},
		{IdleReason::FactoryInsufficientRobotCommandCapacity, "Cannot pull robot due to lack of robot command capacity"},
		{IdleReason::FactoryInsufficientWarehouseSpace, "Cannot pull product due to lack of Warehouse space"},
		{IdleReason::MineExhausted, "Mine exhausted"},
		{IdleReason::MineInactive, "Mine inactive"},
		{IdleReason::InsufficientLuxuryProduct, "Insufficient Luxury Product available"}
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
}


StructureInspector::StructureInspector() :
	Window{"Structure Details"},
	btnClose{"Close", {50, 20}, {this, &StructureInspector::onClose}},
	mIcons{imageCache.load("ui/icons.png")}
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
	auto stringTable = mStructure->createInspectorViewTable();
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
