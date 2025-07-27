#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>
#include <libControls/TextField.h>
#include <libControls/Label.h>

#include <NAS2D/Signal/Delegate.h>


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
		Invalid,
	};

	using CheatDelegate = NAS2D::Delegate<void(CheatCode)>;

	CheatMenu(CheatDelegate cheatHandler);

protected:
	void onVisibilityChange(bool visible) override;

	void onOkay();
	void onEnter(TextField& textField);

private:
	CheatDelegate mCheatHandler;

	Label mLabelCheatCode;
	TextField txtCheatCode;
	Button btnOkay;
};
