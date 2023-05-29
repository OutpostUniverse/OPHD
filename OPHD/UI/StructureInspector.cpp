#include "StructureInspector.h"

#include "../Cache.h"
#include "../Constants/Strings.h"
#include "../MapObjects/Structure.h"
#include "StringTable.h"
#include "TextRender.h"

#include <NAS2D/Utility.h>

#include <stdexcept>


using namespace NAS2D;


StructureInspector::StructureInspector() :
	Window{constants::WindowStructureInspector},
	btnClose{"Close", {this, &StructureInspector::onClose}},
	mIcons{imageCache.load("ui/icons.png")}
{
	size({350, 250});

	btnClose.size({50, 20});
	add(btnClose, {rect().size.x - btnClose.rect().size.x - 5, rect().size.y - btnClose.rect().size.y - 5,});
}


void StructureInspector::structure(Structure* structure)
{
	mStructure = structure;

	if (!mStructure) { return; }

	auto stringTable = buildStringTable();

	auto windowWidth = stringTable.screenRect().size.x + 10;
	size({windowWidth < 350 ? 350 : windowWidth, rect().size.y});

	btnClose.position({positionX() + rect().size.x - 55, btnClose.positionY()});
}


void StructureInspector::onClose()
{
	visible(false);
}


StringTable StructureInspector::buildStringTable() const
{
	StringTable stringTable(4, 6);
	stringTable.position(mRect.position + NAS2D::Vector{5, 25});
	stringTable.setVerticalPadding(5);
	stringTable.setColumnFont(2, stringTable.GetDefaultTitleFont());

	stringTable[{0, 0}].text = "Type:";
	stringTable[{1, 0}].text = mStructure->classDescription();

	if (mStructure->underConstruction())
	{
		stringTable[{2, 0}].text = "Turns Remaining:";
		stringTable[{3, 0}].text = std::to_string(mStructure->turnsToBuild() - mStructure->age());
	}
	else
	{
		stringTable[{2, 0}].text = "Age:";
		stringTable[{3, 0}].text = formatAge();
	}

	stringTable[{0, 1}].text = "Power Required:";
	stringTable[{1, 1}].text = std::to_string(mStructure->energyRequirement());

	stringTable[{2, 1}].text = "State:";
	stringTable[{3, 1}].text = mStructure->stateDescription(mStructure->state());

	stringTable[{3, 2}].text = getDisabledReason();

	if (!mStructure->underConstruction() && !mStructure->destroyed())
	{
		stringTable[{0, 2}].text = "Integrity:";
		stringTable[{1, 2}].text = std::to_string(mStructure->integrity());
	}

	const auto& populationAvailable = mStructure->populationAvailable();
	const auto& populationRequirements = mStructure->populationRequirements();

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

	if (mStructure->hasCrime())
	{
		stringTable[{0, 5}].text = "Crime Rate:";
		stringTable[{1, 5}].text = std::to_string(mStructure->crimeRate()) + "%";
	}

	stringTable.computeRelativeCellPositions();

	return stringTable;
}


void StructureInspector::update()
{
	if (!visible()) { return; }
	Window::update();

	auto& renderer = Utility<Renderer>::get();

	if (mStructure == nullptr)
	{
		throw std::runtime_error("Null pointer to structure within StructureInspector");
	}
	title(mStructure->name());

	auto stringTable = buildStringTable();
	stringTable.draw(renderer);

	drawStructureSpecificTable({stringTable.position().x, stringTable.screenRect().endPoint().y + 25}, renderer);
}

void StructureInspector::drawStructureSpecificTable(NAS2D::Point<int> position, NAS2D::Renderer& renderer)
{
	StringTable stringTable = mStructure->createInspectorViewTable();
	stringTable.computeRelativeCellPositions();
	stringTable.position(position);
	stringTable.draw(renderer);
}

std::string StructureInspector::getDisabledReason() const
{
	if (mStructure->disabled())
	{
		return disabledReason(mStructure->disabledReason());
	}
	else if (mStructure->isIdle())
	{
		return idleReason(mStructure->idleReason());
	}

	return "";
}

std::string StructureInspector::formatAge() const
{
	if (mStructure->ages()) {
		return std::to_string(mStructure->age()) + " of " + std::to_string(mStructure->maxAge());
	}

	return "N/A";
}
