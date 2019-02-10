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


/**
 * Draws resource icons
 */
static void drawResourceIcons(Renderer& r, Image& sheet, int x, int y, int sheet_offset)
{
	r.drawSubImage(sheet, static_cast<float>(x), static_cast<float>(y + 10), 64, static_cast<float>(sheet_offset), static_cast<float>(constants::RESOURCE_ICON_SIZE), static_cast<float>(constants::RESOURCE_ICON_SIZE));
	r.drawSubImage(sheet, static_cast<float>(x), static_cast<float>(y + 26), 80, static_cast<float>(sheet_offset), static_cast<float>(constants::RESOURCE_ICON_SIZE), static_cast<float>(constants::RESOURCE_ICON_SIZE));
	r.drawSubImage(sheet, static_cast<float>(x), static_cast<float>(y + 42), 96, static_cast<float>(sheet_offset), static_cast<float>(constants::RESOURCE_ICON_SIZE), static_cast<float>(constants::RESOURCE_ICON_SIZE));
	r.drawSubImage(sheet, static_cast<float>(x), static_cast<float>(y + 58), 112, static_cast<float>(sheet_offset), static_cast<float>(constants::RESOURCE_ICON_SIZE), static_cast<float>(constants::RESOURCE_ICON_SIZE));
}


/**
 * Draws resource values
 */
static void drawResourceStrings(Renderer& r, Font* f, int x, int y, int res1, int res2, int res3, int res4)
{
	r.drawText(*f, string_format("%i", res1), static_cast<float>(x + 21), static_cast<float>(y + 13), 255, 255, 255);
	r.drawText(*f, string_format("%i", res2), static_cast<float>(x + 21), static_cast<float>(y + 29), 255, 255, 255);
	r.drawText(*f, string_format("%i", res3), static_cast<float>(x + 21), static_cast<float>(y + 45), 255, 255, 255);
	r.drawText(*f, string_format("%i", res4), static_cast<float>(x + 21), static_cast<float>(y + 61), 255, 255, 255);
}



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
	mStructureState = structureStateDescription(mStructure->state());
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
void StructureInspector::drawResourcePool(const std::string& title, ResourcePool& rp, int x, int y)
{
	Renderer& r = Utility<Renderer>::get();

	// Ore
	drawResourceIcons(r, mIcons, x, y, 0);
	drawResourceStrings(r, FONT, x, y, rp.commonMetalsOre(), rp.rareMetalsOre(), rp.commonMineralsOre(), rp.rareMineralsOre());

	// Refined
	drawResourceIcons(r, mIcons, x + 60, y, 16);
	drawResourceStrings(r, FONT, x + 60, y, rp.commonMetals(), rp.rareMetals(), rp.commonMinerals(), rp.rareMinerals());
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
	r.drawText(*FONT, mStructureState, rect().x() + 190 + FONT_BOLD->width("State: "), rect().y() + 25, 255, 255, 255);

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


	if (mStructure->disabled()) { r.drawText(*FONT, disabledReason(mStructure->disabledReason()), rect().x() + 190, rect().y() + 55, 255, 255, 255); }
	
	drawResourcePool("Production Pool", mStructure->production(), static_cast<int>(rect().x() + 5), static_cast<int>(rect().y() + 65));
	drawResourcePool("Storage Pool", mStructure->storage(), static_cast<int>(rect().x() + 190), static_cast<int>(rect().y() + 65));
}
