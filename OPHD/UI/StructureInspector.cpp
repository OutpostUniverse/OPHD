#include "StructureInspector.h"

#include "../Cache.h"
#include "../Constants.h"
#include "../Things/Structures/Structure.h"
#include "StringTable.h"
#include "TextRender.h"

#include <NAS2D/Utility.h>

#include <stdexcept>


using namespace NAS2D;

static const Font* FONT = nullptr;
static const Font* FONT_BOLD = nullptr;


StructureInspector::StructureInspector() :
	btnClose{"Close"},
	mIcons{imageCache.load("ui/icons.png")}
{
	text(constants::WINDOW_STRUCTURE_INSPECTOR);
	init();
}


/**
 * 
 */
StructureInspector::~StructureInspector()
{}


/**
 * 
 */
void StructureInspector::init()
{
	size({350, 200});

	add(&btnClose, 295, 175);
	btnClose.size({50, 20});
	btnClose.click().connect(this, &StructureInspector::btnCloseClicked);

	FONT = &fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	FONT_BOLD = &fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);
}


/**
 * 
 */
void StructureInspector::structure(Structure* structure)
{
	mStructure = structure;
}


/**
 * 
 */
void StructureInspector::btnCloseClicked()
{
	visible(false);
}


/**
 * 
 */
void StructureInspector::update()
{
	if (!visible()) { return; }
	Window::update();

	auto& renderer = Utility<Renderer>::get();

	if (mStructure == nullptr)
	{
		throw std::runtime_error("Null pointer to structure within StructureInspector");
	}
	text(mStructure->name());

	StringTable stringTable(4, 3);
	stringTable.position(mRect.startPoint() + NAS2D::Vector{ 5, 25 });
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

	std::size_t workerIndex = 0;
	std::size_t scientistIndex = 1;

	const auto& populationAvailable = mStructure->populationAvailable();
	const auto& populationRequirements = mStructure->populationRequirements();

	if (populationRequirements[workerIndex] > 0)
	{
		stringTable[{0, 2}].text = "Workers:";
		stringTable[{1, 2}].text = std::to_string(populationAvailable[workerIndex]) + " / " + std::to_string(populationRequirements[workerIndex]);
		stringTable[{1, 2}].textColor = populationAvailable[workerIndex] >= populationRequirements[workerIndex] ? Color::White : Color::Red;
	}

	if (populationRequirements[scientistIndex] > 0)
	{
		stringTable[{0, 3}].text = "Scientists:";
		stringTable[{1, 3}].text = std::to_string(populationAvailable[1]) + " / " + std::to_string(populationRequirements[scientistIndex]);
		stringTable[{1, 3}].textColor = populationAvailable[scientistIndex] >= populationRequirements[scientistIndex] ? Color::White : Color::Red;
	}

	stringTable.computeRelativeCellPositions();
	stringTable.draw(renderer);

	drawStructureSpecificTable({ stringTable.position().x, stringTable.screenRect().endPoint().y + 25 }, renderer);
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
