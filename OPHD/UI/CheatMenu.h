#pragma once

#include "Core/Window.h"
#include "Core/Button.h"
#include "Core/TextField.h"
#include "Core/Label.h"

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
		Invalid
	};

	using CheatSignal = NAS2D::Signal<const std::string&>;
	
	void onOkay();
	void update() override;

	CheatMenu();
	
	static CheatMenu::CheatCode stringToCheatCode(const std::string& cheatCode);
	
	CheatSignal::Source& cheatCodeEntered() { return mSignal; };

private:
	CheatSignal mSignal;

	Label mLabelCheatCode;

	Button btnOkay;

	TextField txtCheatCode;
};