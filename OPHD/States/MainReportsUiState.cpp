#include "MainReportsUiState.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"

#include "../UI/Reports/ReportInterface.h"

#include "../UI/Reports/FactoryReport.h"
#include "../UI/Reports/MineReport.h"
#include "../UI/Reports/ResearchReport.h"
#include "../UI/Reports/WarehouseReport.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <array>


extern NAS2D::Point<int> MOUSE_COORDS;

namespace
{
	const NAS2D::Image* WINDOW_BACKGROUND = nullptr;

	const NAS2D::Font* BIG_FONT = nullptr;
	const NAS2D::Font* BIG_FONT_BOLD = nullptr;
}


/**
 * Enumerated IDs for the navigation panels.
 */
enum NavigationPanel
{
	PANEL_RESEARCH,
	PANEL_PRODUCTION,
	PANEL_WAREHOUSE,
	PANEL_MINING,
	PANEL_SATELLITES,
	PANEL_SPACEPORT,
	PANEL_EXIT,
	PANEL_COUNT
};


/**
 * Represents a navigation panel.
 */
class Panel
{
public:
	void Selected(bool isSelected)
	{
		mIsSelected = isSelected;
		if (UiPanel) { UiPanel->enabled(isSelected); }
	}

	bool Selected() { return mIsSelected; }

public:
	std::string Name;

	const NAS2D::Image* Img = nullptr;

	NAS2D::Point<int> TextPosition;
	NAS2D::Point<int> IconPosition;

	NAS2D::Rectangle<int> Rect;

	ReportInterface* UiPanel = nullptr;

private:
	bool mIsSelected = false;
};


static std::array<Panel, NavigationPanel::PANEL_COUNT> Panels; /**< Array of UI navigation panels. */


/**
 * Computes the panel rectangles for the top nav bar.
 */
static void setPanelRects(int width)
{
	Panels[NavigationPanel::PANEL_EXIT].Rect = {width - 48, 0, 48, 48};
	Panels[NavigationPanel::PANEL_EXIT].IconPosition = {width - 40, 8};

	int remaining_width = width - Panels[NavigationPanel::PANEL_EXIT].Rect.width;
	const auto panelSize = NAS2D::Vector{remaining_width / 6, 48};

	auto panelPosition = NAS2D::Point{0, 0};
	for (std::size_t i = 0; i < NavigationPanel::PANEL_EXIT; ++i)
	{
		auto& panel = Panels[i];
		panel.Rect = NAS2D::Rectangle<int>::Create(panelPosition, panelSize);
		panel.TextPosition = panelPosition + (panelSize - BIG_FONT->size(panel.Name)) / 2 + NAS2D::Vector{20, 0};
		panel.IconPosition = {panel.TextPosition.x - 40, 8};
		panelPosition.x += panelSize.x;
	}
}


/**
 * Draws a UI panel.
 */
static void drawPanel(NAS2D::Renderer& renderer, Panel& panel)
{
	if (panel.Rect.contains(MOUSE_COORDS)) { renderer.drawBoxFilled(panel.Rect, NAS2D::Color{0, 185, 185, 100}); }

	if (panel.Selected())
	{
		renderer.drawBoxFilled(panel.Rect, NAS2D::Color{0, 85, 0});

		if (panel.UiPanel) { panel.UiPanel->update(); }

		renderer.drawText(*BIG_FONT_BOLD, panel.Name, panel.TextPosition, NAS2D::Color{185, 185, 0});
		renderer.drawImage(*panel.Img, panel.IconPosition, 1.0f, NAS2D::Color{185, 185, 0});
	}
	else
	{
		renderer.drawText(*BIG_FONT_BOLD, panel.Name, panel.TextPosition, NAS2D::Color{0, 185, 0});
		renderer.drawImage(*panel.Img, panel.IconPosition, 1.0f, NAS2D::Color{0, 185, 0});
	}
}


MainReportsUiState::MainReportsUiState()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.windowResized().connect({this, &MainReportsUiState::onWindowResized});
	eventHandler.keyDown().connect({this, &MainReportsUiState::onKeyDown});
	eventHandler.mouseButtonDown().connect({this, &MainReportsUiState::onMouseDown});
}


MainReportsUiState::~MainReportsUiState()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.windowResized().disconnect({this, &MainReportsUiState::onWindowResized});
	eventHandler.keyDown().disconnect({this, &MainReportsUiState::onKeyDown});
	eventHandler.mouseButtonDown().disconnect({this, &MainReportsUiState::onMouseDown});

	for (Panel& panel : Panels)
	{
		delete panel.UiPanel;
	}
}


void MainReportsUiState::initialize()
{
	WINDOW_BACKGROUND = &imageCache.load("ui/skin/window_middle_middle.png");

	BIG_FONT = &fontCache.load(constants::FONT_PRIMARY, 16);
	BIG_FONT_BOLD = &fontCache.load(constants::FONT_PRIMARY_BOLD, 16);

	Panels[NavigationPanel::PANEL_EXIT].Img = &imageCache.load("ui/icons/exit.png");

	Panels[NavigationPanel::PANEL_RESEARCH].Img = &imageCache.load("ui/icons/research.png");
	Panels[NavigationPanel::PANEL_RESEARCH].Name = "Research";

	Panels[NavigationPanel::PANEL_PRODUCTION].Img = &imageCache.load("ui/icons/production.png");
	Panels[NavigationPanel::PANEL_PRODUCTION].Name = "Factories";

	Panels[NavigationPanel::PANEL_WAREHOUSE].Img = &imageCache.load("ui/icons/warehouse.png");
	Panels[NavigationPanel::PANEL_WAREHOUSE].Name = "Warehouses";

	Panels[NavigationPanel::PANEL_MINING].Img = &imageCache.load("ui/icons/mine.png");
	Panels[NavigationPanel::PANEL_MINING].Name = "Mines";

	Panels[NavigationPanel::PANEL_SATELLITES].Img = &imageCache.load("ui/icons/satellite.png");
	Panels[NavigationPanel::PANEL_SATELLITES].Name = "Satellites";

	Panels[NavigationPanel::PANEL_SPACEPORT].Img = &imageCache.load("ui/icons/spaceport.png");
	Panels[NavigationPanel::PANEL_SPACEPORT].Name = "Space Ports";

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto size = renderer.size().to<int>();
	setPanelRects(size.x);

	// INIT UI REPORT PANELS
	ReportInterface* factory_report = new FactoryReport();
	Panels[NavigationPanel::PANEL_PRODUCTION].UiPanel = factory_report;
	factory_report->position({0, 48});
	factory_report->size({size.x, size.y - 48});
	factory_report->hide();

	ReportInterface* mining_report = new MineReport();
	Panels[NavigationPanel::PANEL_MINING].UiPanel = mining_report;
	mining_report->position({0, 48});
	mining_report->size({size.x, size.y - 48});
	mining_report->hide();

	ReportInterface* research_report = new ResearchReport();
	Panels[NavigationPanel::PANEL_RESEARCH].UiPanel = research_report;
	research_report->position({0, 48});
	research_report->size({size.x, size.y - 48});
	research_report->hide();

	ReportInterface* warehouse_report = new WarehouseReport();
	Panels[NavigationPanel::PANEL_WAREHOUSE].UiPanel = warehouse_report;
	warehouse_report->position({0, 48});
	warehouse_report->size({size.x, size.y - 48});
	warehouse_report->hide();
}


/**
 * Wrapper activate handler.
 */
void MainReportsUiState::_activate()
{
	for (auto& panel : Panels)
	{
		if (panel.UiPanel)
		{
			panel.UiPanel->fillLists();
			panel.UiPanel->refresh();
		}
	}
}


/**
 * Wrapper deactivate handler.
 */
void MainReportsUiState::_deactivate()
{
	for (auto& panel : Panels)
	{
		if (panel.UiPanel)
		{
			panel.UiPanel->hide();
			panel.UiPanel->clearSelected();
		}

		panel.Selected(false);
	}
}


/**
 * Key down event handler.
 */
void MainReportsUiState::onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier /*mod*/, bool /*repeat*/)
{
	if (!active()) { return; }
	if (key == NAS2D::EventHandler::KeyCode::KEY_ESCAPE) { exit(); }
}


void MainReportsUiState::onMouseDown(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position)
{
	if (!active()) { return; }

	if (!NAS2D::Rectangle{0, 0, NAS2D::Utility<NAS2D::Renderer>::get().size().x, 40}.contains(position)) { return; } // ignore clicks in the UI area.

	if (button == NAS2D::EventHandler::MouseButton::Left)
	{
		for (Panel& panel : Panels)
		{
			bool selected = panel.Rect.contains(MOUSE_COORDS);
			panel.Selected(selected);

			if (panel.UiPanel) { panel.UiPanel->visible(selected); }
		}
	}

	if (Panels[NavigationPanel::PANEL_EXIT].Selected())
	{
		exit();
	}
}


void MainReportsUiState::exit()
{
	deselectAllPanels();

	for (auto& panel : Panels)
	{
		if (panel.UiPanel) { panel.UiPanel->clearSelected(); }
	}

	mReportsUiSignal();
}


/**
 * Window resized event handler.
 */
void MainReportsUiState::onWindowResized(NAS2D::Vector<int> newSize)
{
	setPanelRects(newSize.x);
	for (Panel& panel : Panels)
	{
		if (panel.UiPanel) { panel.UiPanel->size(NAS2D::Vector{newSize.x, newSize.y - 48}); }
	}
}


void MainReportsUiState::deselectAllPanels()
{
	for (auto& panel : Panels)
	{
		panel.Selected(false);
	}
}


/**
 * Structure pointer is assumed to be a factory.
 */
void MainReportsUiState::selectFactoryPanel(Structure* structure)
{
	deselectAllPanels();
	Panels[NavigationPanel::PANEL_PRODUCTION].Selected(true);
	Panels[NavigationPanel::PANEL_PRODUCTION].UiPanel->visible(true);

	Panels[NavigationPanel::PANEL_PRODUCTION].UiPanel->refresh();
	Panels[NavigationPanel::PANEL_PRODUCTION].UiPanel->selectStructure(structure);
}


/**
 * Structure pointer is assumed to be a warehouse.
 */
void MainReportsUiState::selectWarehousePanel(Structure* structure)
{
	deselectAllPanels();
	Panels[NavigationPanel::PANEL_WAREHOUSE].Selected(true);
	Panels[NavigationPanel::PANEL_WAREHOUSE].UiPanel->visible(true);
	Panels[NavigationPanel::PANEL_WAREHOUSE].UiPanel->refresh();
	Panels[NavigationPanel::PANEL_WAREHOUSE].UiPanel->selectStructure(structure);
}


/**
 * Structure pointer is assumed to be a Mine Facility or Smelter.
 */
void MainReportsUiState::selectMinePanel(Structure* structure)
{
	deselectAllPanels();
	Panels[NavigationPanel::PANEL_MINING].Selected(true);
	Panels[NavigationPanel::PANEL_MINING].UiPanel->visible(true);
	Panels[NavigationPanel::PANEL_MINING].UiPanel->refresh();
	Panels[NavigationPanel::PANEL_MINING].UiPanel->selectStructure(structure);
}


void MainReportsUiState::injectTechnology(TechnologyCatalog& catalog, ResearchTracker& tracker)
{
    auto researchPanel = Panels[NavigationPanel::PANEL_RESEARCH].UiPanel;
    static_cast<ResearchReport*>(researchPanel)->injectTechReferences(catalog, tracker);
}


void MainReportsUiState::clearLists()
{
	Panels[NavigationPanel::PANEL_PRODUCTION].UiPanel->fillLists();
	Panels[NavigationPanel::PANEL_WAREHOUSE].UiPanel->fillLists();
	Panels[NavigationPanel::PANEL_MINING].UiPanel->fillLists();
}


/**
 * Gets a list of TakeMeThere signal pointers.
 * 
 * Acts as a pass-through for GameState.
 */
MainReportsUiState::TakeMeThereList MainReportsUiState::takeMeThere()
{
	TakeMeThereList takeMeThereList;
	for (auto& panel : Panels)
	{
		if (panel.UiPanel) { takeMeThereList.push_back(&panel.UiPanel->takeMeThereSignal()); }
	}
	return takeMeThereList;
}


/**
 * Draw
 */
NAS2D::State* MainReportsUiState::update()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	renderer.clearScreen(NAS2D::Color{35, 35, 35});
	renderer.drawBoxFilled(NAS2D::Rectangle{0, 0, renderer.size().x, 48}, NAS2D::Color::Black);

	for (Panel& panel : Panels) { drawPanel(renderer, panel); }

	return this;
}
