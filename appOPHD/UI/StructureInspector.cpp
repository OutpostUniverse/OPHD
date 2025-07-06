#include "StructureInspector.h"

#include "../Cache.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../MapObjects/Structure.h"
#include "StringTable.h"
#include "TextRender.h"

#include <NAS2D/Utility.h>

#include <algorithm>
#include <stdexcept>


using namespace NAS2D;


namespace
{
	const std::map<DisabledReason, std::string> disabledReasonTable =
	{
		{DisabledReason::None, constants::StructureDisabledNone},

		{DisabledReason::Chap, constants::StructureDisabledChap},
		{DisabledReason::Disconnected, constants::StructureDisabledDisconnected},
		{DisabledReason::Energy, constants::StructureDisabledEnergy},
		{DisabledReason::Population, constants::StructureDisabledPopulation},
		{DisabledReason::RefinedResources, constants::StructureDisabledRefinedResources},
		{DisabledReason::StructuralIntegrity, constants::StructureDisabledStructuralIntegrity}
	};

	const std::map<IdleReason, std::string> idleReadonTable =
	{
		{IdleReason::None, constants::StructureIdleNone},

		{IdleReason::PlayerSet, constants::StructureIdlePlayerSet},
		{IdleReason::InternalStorageFull, constants::StructureIdleInternalStorageFull},
		{IdleReason::FactoryProductionComplete, constants::StructureIdleFactoryProductionComplete},
		{IdleReason::FactoryInsufficientResources, constants::StructureIdleFactoryInsufficientResources},
		{IdleReason::FactoryInsufficientRobotCommandCapacity, constants::StructureIdleFactoryInsufficientRobotCommandCapacity},
		{IdleReason::FactoryInsufficientWarehouseSpace, constants::StructureIdleFactoryInsufficnetWarehouseCapacity},
		{IdleReason::MineExhausted, constants::StructureIdleMineExhausted},
		{IdleReason::MineInactive, constants::StructureIdleMineInactive},
		{IdleReason::InsufficientLuxuryProduct, constants::StructureIdleInsufficientLuxuryProduct}
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
		StringTable stringTable{4, 6};

		stringTable[{0, 0}].text = "Type:";
		stringTable[{1, 0}].text = structure.classDescription();

		if (structure.underConstruction())
		{
			stringTable[{2, 0}].text = "Turns Remaining:";
			stringTable[{3, 0}].text = std::to_string(structure.turnsToBuild() - structure.age());
		}
		else
		{
			stringTable[{2, 0}].text = "Age:";
			stringTable[{3, 0}].text = formatAge(structure);
		}

		stringTable[{0, 1}].text = "Power Required:";
		stringTable[{1, 1}].text = std::to_string(structure.energyRequirement());

		stringTable[{2, 1}].text = "State:";
		stringTable[{3, 1}].text = structure.stateDescription(structure.state());

		stringTable[{3, 2}].text = getDisabledReason(structure);

		if (!structure.underConstruction() && !structure.destroyed())
		{
			stringTable[{0, 2}].text = "Integrity:";
			stringTable[{1, 2}].text = std::to_string(structure.integrity());
		}

		const auto& populationAvailable = structure.populationAvailable();
		const auto& populationRequirements = structure.populationRequirements();

		if (populationRequirements.workers > 0)
		{
			stringTable[{0, 3}].text = "Workers:";
			stringTable[{1, 3}].text = std::to_string(populationAvailable.workers) + " / " + std::to_string(populationRequirements.workers);
			stringTable[{1, 3}].textColor = populationAvailable.workers >= populationRequirements.workers ? Color::White : Color::Red;
		}

		if (populationRequirements.scientists > 0)
		{
			stringTable[{0, 4}].text = "Scientists:";
			stringTable[{1, 4}].text = std::to_string(populationAvailable.scientists) + " / " + std::to_string(populationRequirements.scientists);
			stringTable[{1, 4}].textColor = populationAvailable.scientists >= populationRequirements.scientists ? Color::White : Color::Red;
		}

		if (structure.hasCrime())
		{
			stringTable[{0, 5}].text = "Crime Rate:";
			stringTable[{1, 5}].text = std::to_string(structure.crimeRate()) + "%";
		}

		return stringTable;
	}
}


StructureInspector::StructureInspector() :
	Window{constants::WindowStructureInspector},
	btnClose{"Close", {50, 20}, {this, &StructureInspector::onClose}},
	mIcons{imageCache.load("ui/icons.png")}
{
	size({350, 250});
	add(btnClose, area().size - btnClose.size() - NAS2D::Vector{constants::Margin, constants::Margin});
}


void StructureInspector::showStructure(Structure& structure)
{
	mStructure = &structure;
	title(mStructure->name());

	const auto genericStructureAttributes = buildGenericStringTable();
	const auto specificAttributeTablePosition = genericStructureAttributes.area().crossYPoint() + NAS2D::Vector{0, 25};
	const auto specificStructureAttributes = buildSpecificStringTable(specificAttributeTablePosition);

	auto windowSize = NAS2D::Vector{
		std::max({350, genericStructureAttributes.area().size.x, specificStructureAttributes.area().size.x}),
		std::max({250, specificStructureAttributes.area().endPoint().y - genericStructureAttributes.area().position.y + btnClose.size().y})
	} + NAS2D::Vector{constants::Margin, constants::Margin} * 2;

	size(windowSize);

	btnClose.position(area().endPoint() - btnClose.size() - NAS2D::Vector{constants::Margin, constants::Margin});

	show();
}


void StructureInspector::hideStructure(Structure& structure)
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
	stringTable.setColumnFont(2, stringTable.GetDefaultTitleFont());
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


void StructureInspector::drawClientArea() const
{
	const auto genericStructureAttributes = buildGenericStringTable();
	const auto specificAttributeTablePosition = genericStructureAttributes.area().crossYPoint() + NAS2D::Vector{0, 20 + constants::Margin};
	const auto specificStructureAttributes = buildSpecificStringTable(specificAttributeTablePosition);

	auto& renderer = Utility<Renderer>::get();
	genericStructureAttributes.draw(renderer);
	specificStructureAttributes.draw(renderer);
}
