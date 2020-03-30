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
	Renderer& renderer = Utility<Renderer>::get();

	auto position = rect().startPoint() + NAS2D::Vector{10, 85};
	renderer.drawText(*FONT_BOLD, "Population Required", position, NAS2D::Color::White);

	const std::array<std::string, 2> populationTypes{
		"Workers",
		"Scientists"
	};

	position.y() += 20;
	for (std::size_t populationType = 0; populationType < populationTypes.size(); ++populationType) {
		const auto& populationRequirements = mStructure->populationRequirements();
		const auto& populationAvailable = mStructure->populationAvailable();
		if (populationRequirements[populationType] > 0)
		{
			std::string text = populationTypes[populationType] + ": " + std::to_string(populationAvailable[populationType]) + "/" + std::to_string(populationRequirements[populationType]);
			Color color = populationAvailable[populationType] >= populationRequirements[populationType] ? Color::White : Color::Red;
			renderer.drawText(*FONT, text, position, color);
			position.y() += 10;
		}
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
	
	r.drawText(*FONT, "This window is a work in progress", rect().x() + 5, rect().y() + rect().height() - FONT->height() - 5, 255, 255, 255);
}
