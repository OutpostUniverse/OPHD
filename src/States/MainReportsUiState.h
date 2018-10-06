#pragma once

#include "Wrapper.h"

#include "../UI/Core/ComboBox.h"

#include "../Things/Structures/Structure.h"

class MainReportsUiState : public Wrapper
{
public:
	using ReportsUiCallback = NAS2D::Signals::Signal0<void>;
public:
	MainReportsUiState();
	virtual ~MainReportsUiState();

	void selectFactoryPanel(Structure*);

	ReportsUiCallback& hideReports() { return mReportsUiCallback; }

protected:
	void initialize();
	State* update();

private:
	virtual void _deactivate() final;
	virtual void _activate() final;

private:
	void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	void onWindowResized(int w, int h);

	void deselectAllPanels();

private:
	ReportsUiCallback mReportsUiCallback;
};
