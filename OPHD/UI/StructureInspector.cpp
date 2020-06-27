// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "StructureInspector.h"

#include "../Constants.h"
#include "../FontManager.h"

#include "../Things/Structures/Structure.h"

#include <map>
#include <sstream>

#include <NAS2D/Utility.h>


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
	size({350, 200});

	add(&btnClose, 295, 175);
	btnClose.size({50, 20});
	btnClose.click().connect(this, &StructureInspector::btnCloseClicked);


	add(&txtStateDescription, 190, 75);
	txtStateDescription.size({155, 80});
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
	auto& renderer = Utility<Renderer>::get();

	auto position = mRect.startPoint() + NAS2D::Vector{10, 85};
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

	auto& renderer = Utility<Renderer>::get();

	const auto drawTitleText = [&renderer](NAS2D::Point<int> position, const std::string& title, const std::string& text) {
		renderer.drawText(*FONT_BOLD, title, position, NAS2D::Color::White);
		position.x() += FONT_BOLD->width(title);
		renderer.drawText(*FONT, text, position, NAS2D::Color::White);
	};

	auto position = mRect.startPoint() + NAS2D::Vector{5, 25};
	if (mStructure == nullptr)
	{
		drawTitleText(position, "NULLPTR!", "");
		return;
	}
	drawTitleText(position, mStructure->name(), "");

	position.y() += 20;
	drawTitleText(position,"Type: ", mStructureClass);

	position = mRect.startPoint() + NAS2D::Vector{190, 25};
	drawTitleText(position,"State: ", structureStateDescription(mStructure->state()));

	position.y() += 20;
	if (mStructure->underConstruction())
	{
		drawTitleText(position,"Turns Remaining: ", std::to_string(mStructure->turnsToBuild() - mStructure->age()));
	}
	else
	{
		drawTitleText(position,"Age: ", std::to_string(mStructure->age()) + " of " + std::to_string(mStructure->maxAge()));
	}

	drawPopulationRequirements();

	mStructure->drawInspectorView(renderer, mRect);

	position = mRect.startPoint() + NAS2D::Vector{5, static_cast<int>(mRect.height()) - FONT->height() - 5};
	renderer.drawText(*FONT, "This window is a work in progress", position, NAS2D::Color::White);
}
