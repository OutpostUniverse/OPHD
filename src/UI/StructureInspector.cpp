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


StructureInspector::StructureInspector() :
	btnClose{"Close"},
	mIcons{"ui/icons.png"}
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
void StructureInspector::structure(Structure* s)
{
	mStructure = s;
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
	float posX = rect().x() + 10.0f;
	float posY = rect().y() + 85.0f;

	r.drawText(*FONT_BOLD, "Population Required", rect().x() + 10, posY, 255, 255, 255);

	posY += 20;

	if (mStructure->populationRequirements()[0] > 0)
	{
		std::string format = string_format("Workers: %i/%i", mStructure->populationAvailable()[0], mStructure->populationRequirements()[0]);
		Color color = mStructure->populationAvailable()[0] >= mStructure->populationRequirements()[0] ? Color::White : Color::Red;
		r.drawText(*FONT, format, posX, posY, color.red(), color.green(), color.blue());
		posY += 10;
	}

	if (mStructure->populationRequirements()[1] > 0)
	{
		std::string format = string_format("Scientists: %i/%i", mStructure->populationAvailable()[1], mStructure->populationRequirements()[1]);
		Color color = mStructure->populationAvailable()[1] >= mStructure->populationRequirements()[1] ? Color::White : Color::Red;
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

	Renderer& renderer = Utility<Renderer>::get();

	const auto drawTitleText = [&renderer](NAS2D::Point<int> position, const std::string& title, const std::string& text) {
		renderer.drawText(*FONT_BOLD, title, position, NAS2D::Color::White);
		position.x() += FONT_BOLD->width(title);
		renderer.drawText(*FONT, text, position, NAS2D::Color::White);
	};

	auto position = rect().startPoint() + NAS2D::Vector{5, 25};
	if (mStructure == nullptr)
	{
		drawTitleText(position, "NULLPTR!", "");
		return;
	}
	drawTitleText(position, mStructure->name(), "");

	position.y() += 20;
	drawTitleText(position,"Type: ", mStructureClass);

	position = rect().startPoint() + NAS2D::Vector{190, 25};
	drawTitleText(position,"State: ", structureStateDescription(mStructure->state()));

	position.y() += 20;
	if (mStructure->underConstruction())
	{
		drawTitleText(position,"Turns Remaining: ", std::to_string(mStructure->turnsToBuild() - mStructure->age()));
	}
	else
	{
		drawTitleText(position,"Age: ", std::to_string(mStructure->age()));
	}

	drawPopulationRequirements();

	position = rect().startPoint() + NAS2D::Vector{5, static_cast<int>(rect().height()) - FONT->height() - 5};
	renderer.drawText(*FONT, "This window is a work in progress", position, NAS2D::Color::White);
}
