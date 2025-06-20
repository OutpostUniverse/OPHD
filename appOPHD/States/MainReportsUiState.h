#pragma once

#include "Wrapper.h"

#include <NAS2D/Signal/Delegate.h>
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
	using TakeMeThereSignal = NAS2D::Signal<const Structure*>;
	using HideReportsSignal = NAS2D::Signal<>;

	using TakeMeThereDelegate = NAS2D::Delegate<void(const Structure*)>;
	using HideReportsDelegate = NAS2D::Delegate<void()>;

public:
	MainReportsUiState(TakeMeThereDelegate takeMeThereHandler, HideReportsDelegate hideReportsHandler);

	~MainReportsUiState() override;

	void selectFactoryPanel(Structure*);
	void selectWarehousePanel(Structure*);
	void selectMinePanel(Structure*);

	void injectTechnology(TechnologyCatalog&, ResearchTracker&);

	void clearLists();

	void initialize() override;
	State* update() override;

protected:
	void onActivate() override;
	void onDeactivate() override;

	void onWindowResized(NAS2D::Vector<int> newSize);
	void onTakeMeThere(const Structure* structure);
	void onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier mod, bool repeat);
	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onExit();

	void deselectAllPanels();

private:
	const NAS2D::Font& fontMain;

	TakeMeThereSignal mTakeMeThereSignal;
	HideReportsSignal mHideReportsSignal;
};
