#pragma once

#include "Wrapper.h"

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>

#include <vector>

class Structure;
class TechnologyCatalog;
class ResearchTracker;

namespace NAS2D
{
	class Font;
}


class MainReportsUiState : public Wrapper
{
public:
	using ReportsUiSignal = NAS2D::Signal<>;
	using TakeMeThereSignalSource = NAS2D::SignalSource<const Structure*>;
	using TakeMeThereSignalSourceList = std::vector<TakeMeThereSignalSource*>;

public:
	MainReportsUiState();

	~MainReportsUiState() override;

	void selectFactoryPanel(Structure*);
	void selectWarehousePanel(Structure*);
	void selectMinePanel(Structure*);

	void injectTechnology(TechnologyCatalog&, ResearchTracker&);

	void clearLists();

	ReportsUiSignal::Source& hideReports() { return mReportsUiSignal; }
	TakeMeThereSignalSourceList takeMeThere();

protected:
	void initialize() override;
	State* update() override;

private:
	void _deactivate() override;
	void _activate() override;

private:
	void onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier mod, bool repeat);
	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onWindowResized(NAS2D::Vector<int> newSize);

	void deselectAllPanels();

	void exit();

private:
	const NAS2D::Font& fontMain;

	ReportsUiSignal mReportsUiSignal;
};
