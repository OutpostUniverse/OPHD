#pragma once

#include "UI.h"

class DifficultySelect : public Window
{
public:
	using OkClickCallback = NAS2D::Signals::Signal<>;
	using CancelCallback = NAS2D::Signals::Signal<>;

	DifficultySelect();
	~DifficultySelect() override;

	void update() override;
	
	OkClickCallback& ok() { return mOkCallback; }
	CancelCallback& cancel() { return mCancelCallback; }

protected:
	virtual void init();
	void visibilityChanged(bool visible) override;

private:
	void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);
	void enableControls();
	void disableControls();

	void btnOkClicked();
	void cancelled();

	RadioButton rdbBeginner;
	RadioButton rdbEasy;
	RadioButton rdbMedium;
	RadioButton rdbHard;

	Button btnOk;
	OkClickCallback mOkCallback;
	CancelCallback mCancelCallback;
};
