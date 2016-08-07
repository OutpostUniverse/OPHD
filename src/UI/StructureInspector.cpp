#include "StructureInspector.h"

#include <map>
#include <sstream>

extern std::stringstream str_scratch; // FIXME: Ugly hack


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

	str_scratch.str("");
	str_scratch << mStructure->id();
	r.drawText(font(), str_scratch.str(), rect().x() + 190 + mBold.width("Structure ID: "), rect().y() + 25, 0, 0, 0);

	r.drawText(mBold, "Type:", rect().x() + 5, rect().y() + 45, 0, 0, 0);
	r.drawText(font(), TypeTranslationTable[mStructure->type()], rect().x() + 5 + mBold.width("Type: "), rect().y() + 45, 0, 0, 0);

	r.drawText(mBold, "State:", rect().x() + 190, rect().y() + 45, 0, 0, 0);
	r.drawText(font(), StateTranslationTable[mStructure->state()], rect().x() + 190 + mBold.width("Type: "), rect().y() + 45, 0, 0, 0);


	// Cludgy as fuck, holy shit. But I'm lazy so, whatever.
	r.drawText(mBold, "Production Pool", rect().x() + 5, rect().y() + 65, 0, 0, 0);
	r.drawSubImage(mIcons, rect().x() + 5, rect().y() + 75, 64, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 5, rect().y() + 91, 80, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 5, rect().y() + 107, 96, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 5, rect().y() + 123, 112, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 60, rect().y() + 75, 64, 16, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 60, rect().y() + 91, 80, 16, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 60, rect().y() + 107, 96, 16, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 60, rect().y() + 123, 112, 16, 16, 16);

	str_scratch.str("");                                        // SUPER LAZY!!!!!
	str_scratch << mStructure->production().commonMetalsOre() << "        " << mStructure->production().commonMetals();
	r.drawText(font(), str_scratch.str(), rect().x() + 25, rect().y() + 78, 0, 0, 0);

	str_scratch.str("");
	str_scratch << mStructure->production().rareMetalsOre() << "        " << mStructure->production().rareMetals();
	r.drawText(font(), str_scratch.str(), rect().x() + 25, rect().y() + 94, 0, 0, 0);

	str_scratch.str("");
	str_scratch << mStructure->production().commonMineralsOre() << "        " << mStructure->production().commonMinerals();
	r.drawText(font(), str_scratch.str(), rect().x() + 25, rect().y() + 110, 0, 0, 0);

	str_scratch.str("");
	str_scratch << mStructure->production().rareMineralsOre() << "        " << mStructure->production().rareMinerals();
	r.drawText(font(), str_scratch.str(), rect().x() + 25, rect().y() + 126, 0, 0, 0);


	r.drawText(mBold, "Storage Pool", rect().x() + 190, rect().y() + 65, 0, 0, 0);
	r.drawSubImage(mIcons, rect().x() + 190, rect().y() + 75, 64, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 190, rect().y() + 91, 80, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 190, rect().y() + 107, 96, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 190, rect().y() + 123, 112, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 250, rect().y() + 75, 64, 16, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 250, rect().y() + 91, 80, 16, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 250, rect().y() + 107, 96, 16, 16, 16);
	r.drawSubImage(mIcons, rect().x() + 250, rect().y() + 123, 112, 16, 16, 16);

	str_scratch.str("");
	str_scratch << mStructure->storage().commonMetalsOre() << "        " << mStructure->storage().commonMetals();
	r.drawText(font(), str_scratch.str(), rect().x() + 215, rect().y() + 78, 0, 0, 0);

	str_scratch.str("");
	str_scratch << mStructure->storage().rareMetalsOre() << "        " << mStructure->storage().rareMetals();
	r.drawText(font(), str_scratch.str(), rect().x() + 215, rect().y() + 94, 0, 0, 0);

	str_scratch.str("");
	str_scratch << mStructure->storage().commonMineralsOre() << "        " << mStructure->storage().commonMinerals();
	r.drawText(font(), str_scratch.str(), rect().x() + 215, rect().y() + 110, 0, 0, 0);

	str_scratch.str("");
	str_scratch << mStructure->storage().rareMineralsOre() << "        " << mStructure->storage().rareMinerals();
	r.drawText(font(), str_scratch.str(), rect().x() + 215, rect().y() + 126, 0, 0, 0);


	UIContainer::update();
}


void StructureInspector::btnCloseClicked()
{
	visible(false);
}

