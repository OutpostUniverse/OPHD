#pragma once

#include "Wrapper.h"

class Structure;

class MainReportsUiState : public Wrapper
{
public:
	using ReportsUiCallback = NAS2D::Signals::Signal0<void>;
	using TakeMeThere = NAS2D::Signals::Signal1<Structure*>;

public:
	MainReportsUiState();
	virtual ~MainReportsUiState();

	void selectFactoryPanel(Structure*);

	ReportsUiCallback& hideReports() { return mReportsUiCallback; }
	TakeMeThere& takeMeThere();

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

	void exit();

private:
	ReportsUiCallback	mReportsUiCallback;
};
