#include "CheatMenu.h"

#include "../XmlSerializer.h"

using namespace NAS2D;

namespace
{
	static const std::map<std::string, CheatMenu::CheatCode>& cheatCodeTable =
	{
		{"goldrush", CheatMenu::CheatCode::AddResources },      //Add 1000 of each resource.
		{"orderpizza", CheatMenu::CheatCode::AddFood}, //Refill your food supply completely
		{"waveofbabies", CheatMenu::CheatCode::AddChildren}, //Add ten children to the population
		{"savedbythebell", CheatMenu::CheatCode::AddStudents}, //Add ten students to the population
		{"gettowork", CheatMenu::CheatCode::AddWorkers}, //Add ten workers to the population
		{"smartypants", CheatMenu::CheatCode::AddScientists}, //Add ten scientists to the population
		{"electronicoldmen", CheatMenu::CheatCode::AddRetired}, //Add ten retired colonists to the population
		{"beepboop", CheatMenu::CheatCode::AddRobots} //Add a RoboDigger, RoboMiner, and RoboDozer to the robot pool
	};
}

CheatMenu::CheatMenu() :
	Window{"Cheating"},
	btnOkay{"Okay", {this, &CheatMenu::onOkay}}
{
	size({320, 240});

	add(mLabelCheatCode, {5, 25});
	mLabelCheatCode.size({30, 20});
	
	add(btnOkay, {270, 100});
	btnOkay.size({40, 20});
	
	add(txtCheatCode, {10, 100});
	txtCheatCode.size({200, 18});
	txtCheatCode.maxCharacters(50);
}

void CheatMenu::onOkay()
{
	hide();
}

CheatMenu::CheatCode CheatMenu::stringToCheatCode(const std::string& cheatCode)
{
	try
	{
		return stringToEnum(cheatCodeTable, cheatCode);
	}
	catch (const std::runtime_error&)
	{
		return CheatCode::Invalid;
	}
}

void CheatMenu::update()
{
	if (!visible()) { return; }

	Window::update();
}
