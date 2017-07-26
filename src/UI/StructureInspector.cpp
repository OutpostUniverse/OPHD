#include "StructureInspector.h"

#include "../Constants.h"

#include <map>
#include <sstream>

// FIXME: Find a sane place for this as these are usefull throughout the code.
std::map<Structure::StructureClass, std::string> TypeTranslationTable;
std::map<Structure::StructureState, std::string> StateTranslationTable;



StructureInspector::StructureInspector(Font& font) :	mBold("fonts/opensans-bold.ttf", 10),
														mIcons("ui/icons.png"),
														mStructure(nullptr)
{
	Control::font(font);
	text(constants::WINDOW_STRUCTURE_INSPECTOR);
	init();
}


StructureInspector::~StructureInspector()
{
}


void StructureInspector::init()
{
	position(0, 0);
	size(350, 200);

	addControl("btnClose", &btnClose, 295, 175);
	btnClose.font(font());
	btnClose.text("Close");
	btnClose.size(50, 20);
	btnClose.click().connect(this, &StructureInspector::btnCloseClicked);


	// Build translation tables.
	TypeTranslationTable[Structure::CLASS_COMMAND] = "Command";
	TypeTranslationTable[Structure::CLASS_COMM] = "Communication";
	TypeTranslationTable[Structure::CLASS_COMMERCIAL] = "Commercial";
	TypeTranslationTable[Structure::CLASS_ENERGY_PRODUCTION] = "Energy Production";
	TypeTranslationTable[Structure::CLASS_FACTORY] = "Factory";
	TypeTranslationTable[Structure::CLASS_FOOD_PRODUCTION] = "Food Production";
	TypeTranslationTable[Structure::CLASS_LABORATORY] = "Laboratory";
	TypeTranslationTable[Structure::CLASS_LANDER] = "Lander";
	TypeTranslationTable[Structure::CLASS_LIFE_SUPPORT] = "Life Support";
	TypeTranslationTable[Structure::CLASS_MINE] = "Mine Facility";
	TypeTranslationTable[Structure::CLASS_PARK] = "Park / Reservoir";
	TypeTranslationTable[Structure::CLASS_SURFACE_POLICE] = "Police";
	TypeTranslationTable[Structure::CLASS_UNDERGROUND_POLICE] = "Police";
	TypeTranslationTable[Structure::CLASS_RECREATION_CENTER] = "Recreation Center";
	TypeTranslationTable[Structure::CLASS_RECYCLING] = "Recycling";
	TypeTranslationTable[Structure::CLASS_RESIDENCE] = "Residential";
	TypeTranslationTable[Structure::CLASS_SMELTER] = "Raw Ore Processing";
	TypeTranslationTable[Structure::CLASS_STORAGE] = "Storage";
	TypeTranslationTable[Structure::CLASS_TUBE] = "Tube";
	TypeTranslationTable[Structure::CLASS_UNDEFINED] = "UNDEFINED";
	TypeTranslationTable[Structure::CLASS_UNIVERSITY] = "University";

	StateTranslationTable[Structure::UNDER_CONSTRUCTION] = "Under Construction";
	StateTranslationTable[Structure::OPERATIONAL] = "Operational";
	StateTranslationTable[Structure::IDLE] = "Idle";
	StateTranslationTable[Structure::DISABLED] = "Disabled";
	StateTranslationTable[Structure::DESTROYED] = "Destroyed";
}


/**
 * Draws resource icons
 */
void drawResourceIcons(Renderer& r, Image& sheet, int x, int y, int sheet_offset)
{
	r.drawSubImage(sheet, x, y + 10, 64, sheet_offset, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawSubImage(sheet, x, y + 26, 80, sheet_offset, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawSubImage(sheet, x, y + 42, 96, sheet_offset, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawSubImage(sheet, x, y + 58, 112, sheet_offset, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
}


/**
 * Draws resource values
 */
void drawResourceStrings(Renderer& r, Font& f, int x, int y, int res1, int res2, int res3, int res4)
{
	r.drawText(f, string_format("%i", res1), x + 21, y + 13, 255, 255, 255);
	r.drawText(f, string_format("%i", res2), x + 21, y + 29, 255, 255, 255);
	r.drawText(f, string_format("%i", res3), x + 21, y + 45, 255, 255, 255);
	r.drawText(f, string_format("%i", res4), x + 21, y + 61, 255, 255, 255);
}


void StructureInspector::drawResourcePool(const std::string& title, ResourcePool& rp, int x, int y)
{
	Renderer& r = Utility<Renderer>::get();

	//r.drawText(mBold, title, x, y, 0, 0, 0);
	
	// Ore
	drawResourceIcons(r, mIcons, x, y, 0);
	drawResourceStrings(r, font(), x, y, rp.commonMetalsOre(), rp.rareMetalsOre(), rp.commonMineralsOre(), rp.rareMineralsOre());

	// Refined
	drawResourceIcons(r, mIcons, x + 60, y, 16);
	drawResourceStrings(r, font(), x + 60, y, rp.commonMetals(), rp.rareMetals(), rp.commonMinerals(), rp.rareMinerals());
}


void StructureInspector::update()
{
	if (!visible())
		return;

	Window::update();

	Renderer& r = Utility<Renderer>::get();

	if (mStructure == nullptr)
	{
		r.drawText(mBold, "NULLPTR!", rect().x() + 5, rect().y() + 25, 255, 255, 255);
		return;
	}

	r.drawText(mBold, mStructure->name(), rect().x() + 5, rect().y() + 25, 255, 255, 255);

	r.drawText(mBold, "Structure ID:", rect().x() + 190, rect().y() + 25, 255, 255, 255);
	r.drawText(font(), string_format("%i", mStructure->id()), rect().x() + 190 + mBold.width("Structure ID: "), rect().y() + 25, 255, 255, 255);

	r.drawText(mBold, "Type:", rect().x() + 5, rect().y() + 45, 255, 255, 255);
	r.drawText(font(), TypeTranslationTable[mStructure->type()], rect().x() + 5 + mBold.width("Type: "), rect().y() + 45, 255, 255, 255);

	r.drawText(mBold, "State:", rect().x() + 190, rect().y() + 45, 255, 255, 255);
	r.drawText(font(), StateTranslationTable[mStructure->state()], rect().x() + 190 + mBold.width("Type: "), rect().y() + 45, 255, 255, 255);
	
	drawResourcePool("Production Pool", mStructure->production(), rect().x() + 5, rect().y() + 65);
	drawResourcePool("Storage Pool", mStructure->storage(), rect().x() + 190, rect().y() + 65);
}


void StructureInspector::btnCloseClicked()
{
	visible(false);
}

