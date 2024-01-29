#include "CheatMenu.h"

#include <libOPHD/XmlSerializer.h>

using namespace NAS2D;

namespace
{
	enum class CheatCode
	{
		AddResources,
		AddFood,
		AddRobots,
		AddChildren,
		AddStudents,
		AddWorkers,
		AddScientists,
		AddRetired,
		Invalid
	};

	static const std::map<std::string, CheatMenu::CheatCode>& cheatCodeTable =
	{
		{"goldrush", CheatMenu::CheatCode::AddResources },         //Add 1000 of each resource.
		{"orderpizza", CheatMenu::CheatCode::AddFood},             //Refill your food supply completely
		{"waveofbabies", CheatMenu::CheatCode::AddChildren},       //Add ten children to the population
		{"savedbythebell", CheatMenu::CheatCode::AddStudents},     //Add ten students to the population
		{"gettowork", CheatMenu::CheatCode::AddWorkers},           //Add ten workers to the population
		{"smartypants", CheatMenu::CheatCode::AddScientists},      //Add ten scientists to the population
		{"electronicoldmen", CheatMenu::CheatCode::AddRetired},    //Add ten retired colonists to the population
		{"dropchildren", CheatMenu::CheatCode::RemoveChildren},    //Remove ten children from the population
		{"dropstudents", CheatMenu::CheatCode::RemoveStudents},    //Remove ten students from the population
		{"dropworkers", CheatMenu::CheatCode::RemoveWorkers},      //Remove ten workers from the population
		{"dropscientists", CheatMenu::CheatCode::RemoveScientists},//Remove ten scientists from the population
		{"dropretirees", CheatMenu::CheatCode::RemoveRetired},     //Remove ten retired colonists from the population
		{"beepboop", CheatMenu::CheatCode::AddRobots}              //Add a RoboDigger, RoboMiner, and RoboDozer to the robot pool
	};
}


CheatMenu::CheatMenu() :
	Window{"Cheating"},
	btnOkay{"Okay", {this, &CheatMenu::onOkay}}
{
	size({300, 88});

	mLabelCheatCode.size({30, 20});
	mLabelCheatCode.text("Code:");
	add(mLabelCheatCode, {5, 34});

	btnOkay.size({40, 20});
	add(btnOkay, {240, 34});

	txtCheatCode.size({150, 20});
	txtCheatCode.maxCharacters(50);
	add(txtCheatCode, {40, 34});
}


void CheatMenu::onOkay()
{
	mSignal(txtCheatCode.text());
	txtCheatCode.clear();
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
