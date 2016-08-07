#include "StructureInspector.h"

#include <map>
#include <sstream>

// FIXME: Find a sane place for this as these are usefull throughout the code.
std::map<Structure::StructureType, std::string> TypeTranslationTable;
std::map<Structure::StructureState, std::string> StateTranslationTable;



StructureInspector::StructureInspector(Font& font) :	mBold("fonts/ui-bold.png", 7, 9, 0),
														mIcons("ui/icons.png"),
														mStructure(nullptr)
{
	Control::font(font);
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
	btnClose.click().Connect(this, &StructureInspector::btnCloseClicked);


	// Build translation tables.
	TypeTranslationTable[Structure::STRUCTURE_COMMAND] = "Command";
	TypeTranslationTable[Structure::STRUCTURE_COMM] = "Communication";
	TypeTranslationTable[Structure::STRUCTURE_ENERGY_PRODUCTION] = "Energy Production";
	TypeTranslationTable[Structure::STRUCTURE_FACTORY] = "Factory";
	TypeTranslationTable[Structure::STRUCTURE_FOOD_PRODUCTION] = "Food Production";
	TypeTranslationTable[Structure::STRUCTURE_LABORATORY] = "Laboratory";
	TypeTranslationTable[Structure::STRUCTURE_LANDER] = "Lander";
	TypeTranslationTable[Structure::STRUCTURE_LIFE_SUPPORT] = "Life Support";
	TypeTranslationTable[Structure::STRUCTURE_MINE] = "Mine Facility";
	TypeTranslationTable[Structure::STRUCTURE_PARK] = "Park / Reservoir";
	TypeTranslationTable[Structure::STRUCTURE_RECREATION_CENTER] = "Recreation Center";
	TypeTranslationTable[Structure::STRUCTURE_RECYCLING] = "Recycling";
	TypeTranslationTable[Structure::STRUCTURE_RESIDENCE] = "Residential";
	TypeTranslationTable[Structure::STRUCTURE_SMELTER] = "Raw Ore Processing";
	TypeTranslationTable[Structure::STRUCTURE_STORAGE] = "Storage";
	TypeTranslationTable[Structure::STRUCTURE_TUBE] = "Tube";
	TypeTranslationTable[Structure::STRUCTURE_UNDEFINED] = "UNDEFINED";

	StateTranslationTable[Structure::UNDER_CONSTRUCTION] = "Under Construction";
	StateTranslationTable[Structure::OPERATIONAL] = "Operational";
	StateTranslationTable[Structure::IDLE] = "Idle";
	StateTranslationTable[Structure::DISABLED] = "Disabled";
	StateTranslationTable[Structure::DESTROYED] = "Destroyed";
}


void StructureInspector::onMouseDown(MouseButton button, int x, int y)
{
	if (!visible())
		return;
}


void StructureInspector::onMouseUp(MouseButton button, int x, int y)
{
	if (!visible())
		return;
}


/**
 * Draws resource icons
 */
void drawResourceIcons(Renderer& r, Image& sheet, int x, int y, int sheet_offset)
{
	r.drawSubImage(sheet, x, y + 10, 64, sheet_offset, 16, 16);
	r.drawSubImage(sheet, x, y + 26, 80, sheet_offset, 16, 16);
	r.drawSubImage(sheet, x, y + 42, 96, sheet_offset, 16, 16);
	r.drawSubImage(sheet, x, y + 58, 112, sheet_offset, 16, 16);
}


/**
 * Draws resource values
 */
void drawResourceStrings(Renderer& r, Font& f, int x, int y, int res1, int res2, int res3, int res4)
{
	r.drawText(f, string_format("%i", res1), x + 21, y + 13, 0, 0, 0);
	r.drawText(f, string_format("%i", res2), x + 21, y + 29, 0, 0, 0);
	r.drawText(f, string_format("%i", res3), x + 21, y + 45, 0, 0, 0);
	r.drawText(f, string_format("%i", res4), x + 21, y + 61, 0, 0, 0);
}


void StructureInspector::drawResourcePool(const std::string& title, ResourcePool& rp, int x, int y)
{
	Renderer& r = Utility<Renderer>::get();

	r.drawText(mBold, title, x, y, 0, 0, 0);
	
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

	Renderer& r = Utility<Renderer>::get();

	r.drawBoxFilled(rect(), COLOR_SILVER.red(), COLOR_SILVER.green(), COLOR_SILVER.blue());
	r.drawBoxFilled(rect().x(), rect().y(), rect().w(), 19.0f, COLOR_SILVER.red() + 25, COLOR_SILVER.green() + 25, COLOR_SILVER.blue() + 25);
	r.drawBox(rect(), 0, 0, 0);

	r.drawText(mBold, "Structure Inspector", rect().x() + 5, rect().y() + 5, 0, 0, 0);

	if (!mStructure)
	{
		r.drawText(mBold, "NULLPTR!", rect().x() + 5, rect().y() + 25, 0, 0, 0);
		return;
	}

	r.drawText(mBold, mStructure->name(), rect().x() + 5, rect().y() + 25, 0, 0, 0);
	r.drawText(mBold, "Structure ID:", rect().x() + 190, rect().y() + 25, 0, 0, 0);

	r.drawText(font(), string_format("%i", mStructure->id()), rect().x() + 190 + mBold.width("Structure ID: "), rect().y() + 25, 0, 0, 0);

	r.drawText(mBold, "Type:", rect().x() + 5, rect().y() + 45, 0, 0, 0);
	r.drawText(font(), TypeTranslationTable[mStructure->type()], rect().x() + 5 + mBold.width("Type: "), rect().y() + 45, 0, 0, 0);

	r.drawText(mBold, "State:", rect().x() + 190, rect().y() + 45, 0, 0, 0);
	r.drawText(font(), StateTranslationTable[mStructure->state()], rect().x() + 190 + mBold.width("Type: "), rect().y() + 45, 0, 0, 0);
	
	drawResourcePool("Production Pool", mStructure->production(), rect().x() + 5, rect().y() + 65);
	drawResourcePool("Storage Pool", mStructure->storage(), rect().x() + 190, rect().y() + 65);

	UIContainer::update();
}


void StructureInspector::btnCloseClicked()
{
	visible(false);
}

