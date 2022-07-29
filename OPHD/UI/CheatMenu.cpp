#include "CheatMenu.h"

#include "../XmlSerializer.h"

using namespace NAS2D;

namespace
{
	const std::map<std::string, CheatCode> cheatCodeTable =
	{
		{"goldrush", AddResources },      //Add 1000 of each resource.
		{"orderpizza", AddFood },         //Refill your food supply completely
		{"waveofbabies", AddChildren},    //Add ten children to the population
		{"savedbythebell", AddStudents},  //Add ten students to the population
		{"gettowork", AddWorkers},        //Add ten workers to the population
		{"smartypants", AddScientists},   //Add ten scientists to the population
		{"electronicoldmen", AddRetired}, //Add ten retired colonists to the population
		{"beepboop", AddRobots}           //Add a RoboDigger, RoboMiner, and RoboDozer to the robot pool
	};
}

CheatMenu::CheatMenu() :
	Window{"Cheat menu"},
{
	size({320, 240});

	add(mLabelCheatCode, {5, 25});
	mLabelCheatCode.size({600, 20});

	add(btnOk, {270, 100});
	btnOkay.size({40, 20});

	add(txtCheatCode, {10, 100});
	txtCheatCode.size({200, 18});
	txtCheatCode.maxCharacters(50);
}

void CheatMenu::onOkay()
{
	hide();
}

void CheatMenu::update()
{
	if (!visible()) { return; }

	Window::update();
}

static CheatCode CheatMenu::stringToCheatCode(const std::string& cheatCode)
{
	try
	{
		stringToEnum(cheatCodeTable, cheatCode),
	}
	catch (const runtime_error& e)
	{
		return CheatCode::Invalid;
	}
}
