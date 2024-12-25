#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>
#include <libControls/TextField.h>
#include <libControls/Label.h>

#include <NAS2D/Signal/Signal.h>

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

	using CheatSignal = NAS2D::Signal<const std::string&>;

	void onOkay();
	void update() override;

	CheatMenu();

	static CheatMenu::CheatCode stringToCheatCode(const std::string& cheatCode);

	CheatSignal::Source& cheatCodeEntered() { return mSignal; }

private:
	CheatSignal mSignal;

	Label mLabelCheatCode;

	Button btnOkay;

	TextField txtCheatCode;
};
