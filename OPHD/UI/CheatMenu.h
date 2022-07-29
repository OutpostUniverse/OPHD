#pragma once

#include "Core/Window.h"
#include "Core/Button.h"
#include "Core/TextField.h"
#include "Core/Label.h"

#include <NAS2D/Signal/Signal.h>

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
		Invalid
	}
	using CheatSignal = NAS2D::Signal<const std::string&>;

	Signal::Source& cheatCodeEntered() { return mSignal; }


private:

	CheatSignal mSignal;

	Label mLabelCheatCode;

	Button btnOkay{"Okay", {this, &CheatMenu::onOkay}};

	TextField txtCheatCode;
};
