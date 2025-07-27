#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>
#include <libControls/TextField.h>
#include <libControls/Label.h>

#include <NAS2D/Signal/Delegate.h>

#include <map>

class CheatMenu : public Window
{
public:
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
		RemoveChildren,
		RemoveStudents,
		RemoveWorkers,
		RemoveScientists,
		RemoveRetired,
		Invalid
	};

	using CheatDelegate = NAS2D::Delegate<void(CheatCode)>;

	CheatMenu(CheatDelegate cheatHandler);

	void onOkay();

	static CheatMenu::CheatCode stringToCheatCode(const std::string& cheatCode);

private:
	CheatDelegate mCheatHandler;

	Label mLabelCheatCode;
	TextField txtCheatCode;
	Button btnOkay;
};
