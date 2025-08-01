#pragma once

#include "Wrapper.h"

#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/Math/Point.h>

#include <cstdint>


class Structure;
class TechnologyCatalog;
class ResearchTracker;

namespace NAS2D
{
	enum class KeyModifier : uint16_t;
	enum class KeyCode : uint32_t;
	enum class MouseButton;

	class Font;
}


class ReportsState : public Wrapper
{
public:
	using TakeMeThereDelegate = NAS2D::Delegate<void(const Structure*)>;
	using ShowReportsDelegate = NAS2D::Delegate<void()>;
	using HideReportsDelegate = NAS2D::Delegate<void()>;

public:
	ReportsState(TakeMeThereDelegate takeMeThereHandler, ShowReportsDelegate showReportsHandler, HideReportsDelegate hideReportsHandler);

	~ReportsState() override;

	void showReport();
	void showReport(Structure& structure);

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

	TakeMeThereDelegate mTakeMeThereHandler;
	ShowReportsDelegate mShowReportsHandler;
	HideReportsDelegate mHideReportsHandler;
};
