#include "ReportsState.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"
#include "../StructureManager.h"

#include "../UI/Reports/Report.h"

#include "../UI/Reports/FactoryReport.h"
#include "../UI/Reports/MineReport.h"
#include "../UI/Reports/ResearchReport.h"
#include "../UI/Reports/SatellitesReport.h"
#include "../UI/Reports/SpaceportsReport.h"
#include "../UI/Reports/WarehouseReport.h"

#include <NAS2D/EnumKeyCode.h>
#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>

#include <array>


class Structure;

extern NAS2D::Point<int> MOUSE_COORDS;

namespace
{
	enum class NavigationPanel
	{
		Research,
		Production,
		Warehouse,
		Mines,
		Satellites,
		Spaceports,
		Exit
	};

	constexpr auto ResearchPanelIndex = static_cast<size_t>(NavigationPanel::Research);
	constexpr auto ExitPanelIndex = static_cast<size_t>(NavigationPanel::Exit);


	class Panel
	{
	public:
		Panel() = default;
		Panel(Report* newReport, std::string newName, const NAS2D::Image* newIcon) :
			report{newReport},
			name{newName},
			icon{newIcon}
		{}

		void select(Structure& structure)
		{
			selected(true);
			report->visible(true);
			report->refresh();
			report->selectStructure(structure);
		}

		void selected(bool isSelected)
		{
			mIsSelected = isSelected;
			if (report)
			{
				report->enabled(isSelected);
			}
		}

		bool selected() const
		{
			return mIsSelected;
		}

	public:
		Report* report = nullptr;
		std::string name;
		const NAS2D::Image* icon = nullptr;

		NAS2D::Rectangle<int> tabArea;
		NAS2D::Point<int> textPosition;
		NAS2D::Point<int> iconPosition;

	private:
		bool mIsSelected = false;
	};


	static std::array<Panel, 7> panels;


	void initializePanels(ReportsState::TakeMeThereDelegate takeMeThereHandler)
	{
		const auto& structureManager = NAS2D::Utility<StructureManager>::get();
		/* NOTE: Matches the order in enum NavigationPanel */
		panels = std::array<Panel, 7>{
			Panel{new ResearchReport(takeMeThereHandler), "Research", &imageCache.load("ui/icons/research.png")},
			Panel{new FactoryReport(structureManager, takeMeThereHandler), "Factories", &imageCache.load("ui/icons/production.png")},
			Panel{new WarehouseReport(takeMeThereHandler), "Warehouses", &imageCache.load("ui/icons/warehouse.png")},
			Panel{new MineReport(takeMeThereHandler), "Mines", &imageCache.load("ui/icons/mine.png")},
			Panel{new SatellitesReport(takeMeThereHandler), "Satellites", &imageCache.load("ui/icons/satellite.png")},
			Panel{new SpaceportsReport(takeMeThereHandler), "Space Ports", &imageCache.load("ui/icons/spaceport.png")},
			Panel{nullptr, "", &imageCache.load("ui/icons/exit.png")}
		};

		for (auto& panel : panels)
		{
			auto* report = panel.report;
			if (report)
			{
				report->hide();
			}
		}
	}


	void onResizeTabBar(int width, const NAS2D::Font& font)
	{
		auto& exitPanel = panels[ExitPanelIndex];
		exitPanel.tabArea = {{width - 48, 0}, {48, 48}};
		exitPanel.iconPosition = {width - 40, 8};

		int remaining_width = width - exitPanel.tabArea.size.x;
		const auto panelSize = NAS2D::Vector{remaining_width / 6, 48};

		auto panelPosition = NAS2D::Point{0, 0};
		for (std::size_t i = 0; i < panels.size() - 1; ++i)
		{
			auto& panel = panels[i];
			panel.tabArea = NAS2D::Rectangle{panelPosition, panelSize};
			panel.textPosition = panelPosition + (panelSize - font.size(panel.name)) / 2 + NAS2D::Vector{20, 0};
			panel.iconPosition = {panel.textPosition.x - 40, 8};
			panelPosition.x += panelSize.x;
		}
	}


	void drawPanel(NAS2D::Renderer& renderer, Panel& panel, const NAS2D::Font& font)
	{
		if (panel.tabArea.contains(MOUSE_COORDS))
		{
			renderer.drawBoxFilled(panel.tabArea, constants::HighlightColor);
		}

		auto drawColor = panel.selected() ? constants::PrimaryColor : constants::SecondaryColor;

		if (panel.selected())
		{
			renderer.drawBoxFilled(panel.tabArea, constants::PrimaryColorVariant);

			if (panel.report)
			{
				panel.report->update();
			}
		}

		renderer.drawText(font, panel.name, panel.textPosition, drawColor);
		renderer.drawImage(*panel.icon, panel.iconPosition, 1.0f, drawColor);
	}
}


ReportsState::ReportsState(TakeMeThereDelegate takeMeThereHandler, ShowReportsDelegate showReportsHandler, HideReportsDelegate hideReportsHandler) :
	fontMain{fontCache.load(constants::FontPrimaryBold, 16)},
	mTakeMeThereHandler{takeMeThereHandler},
	mShowReportsHandler{showReportsHandler},
	mHideReportsHandler{hideReportsHandler}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.windowResized().connect({this, &ReportsState::onWindowResized});
	eventHandler.keyDown().connect({this, &ReportsState::onKeyDown});
	eventHandler.mouseButtonDown().connect({this, &ReportsState::onMouseDown});
}


ReportsState::~ReportsState()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.windowResized().disconnect({this, &ReportsState::onWindowResized});
	eventHandler.keyDown().disconnect({this, &ReportsState::onKeyDown});
	eventHandler.mouseButtonDown().disconnect({this, &ReportsState::onMouseDown});

	for (Panel& panel : panels)
	{
		delete panel.report;
	}
}


void ReportsState::initialize()
{
	initializePanels({this, &ReportsState::onTakeMeThere});
	const auto size = NAS2D::Utility<NAS2D::Renderer>::get().size().to<int>();
	onWindowResized(size);
}


void ReportsState::onActivate()
{
	for (auto& panel : panels)
	{
		if (panel.report)
		{
			panel.report->fillLists();
			panel.report->refresh();
			panel.report->show();
		}
	}
}


void ReportsState::onDeactivate()
{
	for (auto& panel : panels)
	{
		if (panel.report)
		{
			panel.report->hide();
			panel.report->clearSelected();
		}

		panel.selected(false);
	}
}


void ReportsState::onWindowResized(NAS2D::Vector<int> newSize)
{
	onResizeTabBar(newSize.x, fontMain);
	for (Panel& panel : panels)
	{
		if (panel.report)
		{
			panel.report->area({{0, 48}, NAS2D::Vector{newSize.x, newSize.y - 48}});
		}
	}
}


void ReportsState::onTakeMeThere(const Structure* structure)
{
	// Ignore events that come from an inactive tab
	// This is a bit of a hack to fix a bug where non-visible reports respond to events
	// It would be cleaner to have non-visible reports not process user input
	// A better fix relies up reworking event dispatch
	for (auto& panel : panels)
	{
		if (panel.report)
		{
			if (panel.report->canView(*structure) && !panel.selected())
			{
				return;
			}
		}
	}

	if (mTakeMeThereHandler) { mTakeMeThereHandler(structure); }
}


void ReportsState::onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier /*mod*/, bool /*repeat*/)
{
	if (!active())
	{
		return;
	}

	if (key == NAS2D::KeyCode::Escape)
	{
		onExit();
	}
}


void ReportsState::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!active())
	{
		return;
	}

	// ignore clicks in the UI area.
	if (!NAS2D::Rectangle<int>{{0, 0}, {NAS2D::Utility<NAS2D::Renderer>::get().size().x, 40}}.contains(position))
	{
		return;
	}

	if (button == NAS2D::MouseButton::Left)
	{
		for (Panel& panel : panels)
		{
			bool selected = panel.tabArea.contains(MOUSE_COORDS);
			panel.selected(selected);

			if (panel.report)
			{
				panel.report->visible(selected);
			}
		}
	}

	if (panels[ExitPanelIndex].selected())
	{
		onExit();
	}
}


void ReportsState::onExit()
{
	deselectAllPanels();

	for (auto& panel : panels)
	{
		if (panel.report)
		{
			panel.report->clearSelected();
		}
	}

	if (mHideReportsHandler) { mHideReportsHandler(); }
}


void ReportsState::deselectAllPanels()
{
	for (auto& panel : panels)
	{
		panel.selected(false);
	}
}


void ReportsState::showReport()
{
	if (mShowReportsHandler) { mShowReportsHandler(); }
}


void ReportsState::showReport(Structure& structure)
{
	for (auto& panel : panels)
	{
		if (panel.report)
		{
			if (panel.report->canView(structure))
			{
				deselectAllPanels();
				panel.select(structure);
				if (mShowReportsHandler) { mShowReportsHandler(); }
				return;
			}
		}
	}
}


void ReportsState::injectTechnology(TechnologyCatalog& catalog, ResearchTracker& tracker)
{
	auto* researchPanel = panels[ResearchPanelIndex].report;
	dynamic_cast<ResearchReport&>(*researchPanel).injectTechReferences(catalog, tracker);
}


void ReportsState::clearLists()
{
	for (auto& panel : panels)
	{
		if (panel.report)
		{
			panel.report->fillLists();
		}
	}
}


NAS2D::State* ReportsState::update()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	renderer.clearScreen(NAS2D::Color{35, 35, 35});
	renderer.drawBoxFilled(NAS2D::Rectangle<int>{{0, 0}, {renderer.size().x, 48}}, NAS2D::Color::Black);

	for (Panel& panel : panels)
	{
		drawPanel(renderer, panel, fontMain);
	}

	return this;
}
