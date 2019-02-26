// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "StructureInspector.h"

#include "../Constants.h"
#include "../FontManager.h"

#include <map>
#include <sstream>


using namespace NAS2D;

static Font* FONT = nullptr;
static Font* FONT_BOLD = nullptr;


StructureInspector::StructureInspector() : mIcons("ui/icons.png")
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
	size(350, 200);

	add(&btnClose, 295, 175);
	btnClose.text("Close");
	btnClose.size(50, 20);
	btnClose.click().connect(this, &StructureInspector::btnCloseClicked);


	add(&txtStateDescription, 190, 75);
	txtStateDescription.size(155, 80);
	txtStateDescription.font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);

	FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	FONT_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);
}


/**
 * 
 */
void StructureInspector::structure(Structure* _st)
{
	mStructure = _st;
	mStructureClass = structureClassDescription(mStructure->structureClass());
	check();
}


/**
 * Forces a state check. Used to update text area for descriptions.
 */
void StructureInspector::check()
{
	if (!mStructure) { return; }

	txtStateDescription.text("");

	if (mStructure->disabled() || mStructure->destroyed()) { txtStateDescription.text(disabledReason(mStructure->disabledReason())); }
	else if (mStructure->isIdle()) { txtStateDescription.text(idleReason(mStructure->idleReason())); }

	txtStateDescription.visible(!txtStateDescription.text().empty());
}


/**
 * 
 */
void StructureInspector::btnCloseClicked()
{
	visible(false);
}


void StructureInspector::drawPopulationRequirements()
{
	Renderer& r = Utility<Renderer>::get();
	int posX = rect().x() + 10;
	int posY = rect().y() + 85;

	r.drawText(*FONT_BOLD, "Population Required", rect().x() + 10, posY, 255, 255, 255);

	posY += 20;

	if (mStructure->populationRequirements()[0] > 0)
	{
		std::string format = string_format("Workers: %i/%i", mStructure->populationAvailable()[0], mStructure->populationRequirements()[0]);
		Color_4ub color = mStructure->populationAvailable()[0] >= mStructure->populationRequirements()[0] ? COLOR_WHITE : COLOR_RED;
		r.drawText(*FONT, format, posX, posY, color.red(), color.green(), color.blue());
		posY += 10;
	}

	if (mStructure->populationRequirements()[1] > 0)
	{
		std::string format = string_format("Scientists: %i/%i", mStructure->populationAvailable()[1], mStructure->populationRequirements()[1]);
		Color_4ub color = mStructure->populationAvailable()[1] >= mStructure->populationRequirements()[1] ? COLOR_WHITE : COLOR_RED;
		r.drawText(*FONT, format, posX, posY, color.red(), color.green(), color.blue());
	}
}


/**
 * 
 */
void StructureInspector::update()
{
	if (!visible()) { return; }
	Window::update();

	Renderer& r = Utility<Renderer>::get();

	if (mStructure == nullptr)
	{
		r.drawText(*FONT_BOLD, "NULLPTR!", rect().x() + 5, rect().y() + 25, 255, 255, 255);
		return;
	}

	r.drawText(*FONT_BOLD, mStructure->name(), rect().x() + 5, rect().y() + 25, 255, 255, 255);

	r.drawText(*FONT_BOLD, "Type:", rect().x() + 5, rect().y() + 45, 255, 255, 255);
	r.drawText(*FONT, mStructureClass, rect().x() + 5 + FONT_BOLD->width("Type: "), rect().y() + 45, 255, 255, 255);

	r.drawText(*FONT_BOLD, "State:", rect().x() + 190, rect().y() + 25, 255, 255, 255);
	r.drawText(*FONT, structureStateDescription(mStructure->state()), rect().x() + 190 + FONT_BOLD->width("State: "), rect().y() + 25, 255, 255, 255);

	if (mStructure->underConstruction())
	{
		r.drawText(*FONT_BOLD, "Turns Remaining:", rect().x() + 190, rect().y() + 45, 255, 255, 255);
		r.drawText(*FONT, std::to_string(mStructure->turnsToBuild() - mStructure->age()), rect().x() + 190 + FONT_BOLD->width("Turns Remaining: "), rect().y() + 45, 255, 255, 255);
	}
	else
	{
		r.drawText(*FONT_BOLD, "Age:", rect().x() + 190, rect().y() + 45, 255, 255, 255);
		r.drawText(*FONT, std::to_string(mStructure->age()), rect().x() + 190 + FONT_BOLD->width("Age: "), rect().y() + 45, 255, 255, 255);
	}

	drawPopulationRequirements();


	if (mStructure->disabled())
	{
		r.drawText(*FONT, disabledReason(mStructure->disabledReason()), rect().x() + 190, rect().y() + 75, 255, 255, 255);
	}
	
	r.drawText(*FONT, "This window is a work in progress", rect().x() + 5, rect().y() + rect().height() - FONT->height() - 5, 255, 255, 255);
}
