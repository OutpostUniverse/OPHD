#pragma once

#include "Wrapper.h"

#include <vector>


class Structure;

class MainReportsUiState : public Wrapper
{
public:
	using ReportsUiCallback = NAS2D::Signals::Signal<>;
	using TakeMeThere = NAS2D::Signals::Signal1<Structure*>;
	using TakeMeThereList = std::vector<TakeMeThere*>;

public:
	MainReportsUiState();
	virtual ~MainReportsUiState();

	void selectFactoryPanel(Structure*);
	void selectWarehousePanel(Structure*);
	void selectMinePanel(Structure*);

	void clearLists();

	ReportsUiCallback& hideReports() { return mReportsUiCallback; }
	TakeMeThereList takeMeThere();

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
