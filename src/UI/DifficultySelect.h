#pragma once

#include "UI.h"

class DifficultySelect : public Window
{
public:
	typedef NAS2D::Signals::Signal0<void> OkClickCallback;

	DifficultySelect();
	virtual ~DifficultySelect();

	virtual void update() override;
	
	OkClickCallback& okClicked() { return mCallback; }

protected:
	virtual void init();
	virtual void visibilityChanged(bool visible) override;

private:
	void enableControls();
	void disableControls();

	void btnOkClicked();

	RadioButton rdbBeginner;
	RadioButton rdbEasy;
	RadioButton rdbMedium;
	RadioButton rdbHard;

	Button btnOk;
	OkClickCallback mCallback;
};
