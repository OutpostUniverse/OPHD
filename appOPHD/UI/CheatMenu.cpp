#include "CheatMenu.h"

#include <libOPHD/XmlSerializer.h>

#include <algorithm>
#include <ranges>
#include <string>
#include <map>


namespace
{
	const std::map<std::string, CheatMenu::CheatCode>& cheatCodeTable =
	{
		{"goldrush", CheatMenu::CheatCode::AddResources },         // Add 1000 of each resource.
		{"orderpizza", CheatMenu::CheatCode::AddFood},             // Refill your food supply completely
		{"waveofbabies", CheatMenu::CheatCode::AddChildren},       // Add ten children to the population
		{"savedbythebell", CheatMenu::CheatCode::AddStudents},     // Add ten students to the population
		{"gettowork", CheatMenu::CheatCode::AddWorkers},           // Add ten workers to the population
		{"smartypants", CheatMenu::CheatCode::AddScientists},      // Add ten scientists to the population
		{"electronicoldmen", CheatMenu::CheatCode::AddRetired},    // Add ten retired colonists to the population
		{"dropchildren", CheatMenu::CheatCode::RemoveChildren},    // Remove ten children from the population
		{"dropstudents", CheatMenu::CheatCode::RemoveStudents},    // Remove ten students from the population
		{"dropworkers", CheatMenu::CheatCode::RemoveWorkers},      // Remove ten workers from the population
		{"dropscientists", CheatMenu::CheatCode::RemoveScientists},// Remove ten scientists from the population
		{"dropretirees", CheatMenu::CheatCode::RemoveRetired},     // Remove ten retired colonists from the population
		{"beepboop", CheatMenu::CheatCode::AddRobots}              // Add a RoboDigger, RoboMiner, and RoboDozer to the robot pool
	};

	const auto maxCheatLength = std::ranges::max(std::views::transform(std::views::keys(cheatCodeTable), &std::string::size));


	CheatMenu::CheatCode stringToCheatCode(const std::string& cheatCode)
	{
		try
		{
			return stringToEnum(cheatCodeTable, cheatCode);
		}
		catch (const std::runtime_error&)
		{
			return CheatMenu::CheatCode::Invalid;
		}
	}
}


CheatMenu::CheatMenu(CheatDelegate cheatHandler) :
	Window{"Cheating"},
	mCheatHandler{cheatHandler},
	mLabelCheatCode{"Code:"},
	txtCheatCode{maxCheatLength},
	btnOkay{"Okay", {this, &CheatMenu::onOkay}}
{
	btnOkay.size(btnOkay.size() + NAS2D::Vector{6, 2});

	const auto positionY = sWindowTitleBarHeight + 10;
	add(mLabelCheatCode, {10, positionY});
	add(txtCheatCode, {mLabelCheatCode.area().endPoint().x + 6, positionY});
	add(btnOkay, {txtCheatCode.area().endPoint().x + 6, positionY});

	size({btnOkay.area().endPoint().x + 10, positionY + txtCheatCode.size().y + 10});
	// Transfer focus to text field
	bringToFront(&txtCheatCode);
}


void CheatMenu::onOkay()
{
	if (mCheatHandler) { mCheatHandler(stringToCheatCode(txtCheatCode.text())); }
	txtCheatCode.clear();
	hide();
	// Transfer focus back to text field (from "Okay" button)
	bringToFront(&txtCheatCode);
}
