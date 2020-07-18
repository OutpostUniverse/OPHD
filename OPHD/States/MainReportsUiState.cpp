// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MainReportsUiState.h"

#include "../Constants.h"
#include "../FontManager.h"

#include "../UI/Reports/ReportInterface.h"

#include "../UI/Reports/FactoryReport.h"
#include "../UI/Reports/WarehouseReport.h"

#include <array>


using namespace NAS2D;


extern Point<int> MOUSE_COORDS;

static Image* WINDOW_BACKGROUND = nullptr;

Font* BIG_FONT = nullptr;
Font* BIG_FONT_BOLD = nullptr;


/**
 * Enumerated ID's for the navigation panels.
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
	void Selected(bool _b)
	{
		_selected = _b;
		if (UiPanel) { UiPanel->enabled(_b); }
	}
	
	bool Selected() { return _selected; }

public:
	std::string Name;

	Image* Img = nullptr;

	Point<int> TextPosition;
	Point<int> IconPosition;

	Rectangle<int> Rect;

	ReportInterface* UiPanel = nullptr;

private:
	bool _selected = false;
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
	int text_y_position = (panelSize.y - BIG_FONT->height()) / 2;

	auto panelPosition = NAS2D::Point{0, 0};
	Panels[NavigationPanel::PANEL_RESEARCH].Rect = NAS2D::Rectangle<int>::Create(panelPosition, panelSize);
	Panels[NavigationPanel::PANEL_RESEARCH].TextPosition = {panelSize.x / 2 - BIG_FONT->width(Panels[NavigationPanel::PANEL_RESEARCH].Name) / 2 + 20, text_y_position};
	Panels[NavigationPanel::PANEL_RESEARCH].IconPosition = {Panels[NavigationPanel::PANEL_RESEARCH].TextPosition.x - 40, 8};

	panelPosition.x += panelSize.x;
	Panels[NavigationPanel::PANEL_PRODUCTION].Rect = NAS2D::Rectangle<int>::Create(panelPosition, panelSize);
	Panels[NavigationPanel::PANEL_PRODUCTION].TextPosition = {Panels[NavigationPanel::PANEL_PRODUCTION].Rect.x + panelSize.x / 2 - BIG_FONT->width(Panels[NavigationPanel::PANEL_PRODUCTION].Name) / 2 + 20, text_y_position};
	Panels[NavigationPanel::PANEL_PRODUCTION].IconPosition = {Panels[NavigationPanel::PANEL_PRODUCTION].TextPosition.x - 40, 8};

	panelPosition.x += panelSize.x;
	Panels[NavigationPanel::PANEL_WAREHOUSE].Rect = NAS2D::Rectangle<int>::Create(panelPosition, panelSize);
	Panels[NavigationPanel::PANEL_WAREHOUSE].TextPosition = {Panels[NavigationPanel::PANEL_WAREHOUSE].Rect.x + panelSize.x / 2 - BIG_FONT->width(Panels[NavigationPanel::PANEL_WAREHOUSE].Name) / 2 + 20, text_y_position};
	Panels[NavigationPanel::PANEL_WAREHOUSE].IconPosition = {Panels[NavigationPanel::PANEL_WAREHOUSE].TextPosition.x - 40, 8};

	panelPosition.x += panelSize.x;
	Panels[NavigationPanel::PANEL_MINING].Rect = NAS2D::Rectangle<int>::Create(panelPosition, panelSize);
	Panels[NavigationPanel::PANEL_MINING].TextPosition = {Panels[NavigationPanel::PANEL_MINING].Rect.x + panelSize.x / 2 - BIG_FONT->width(Panels[NavigationPanel::PANEL_MINING].Name) / 2 + 20, text_y_position};
	Panels[NavigationPanel::PANEL_MINING].IconPosition = {Panels[NavigationPanel::PANEL_MINING].TextPosition.x - 40, 8};

	panelPosition.x += panelSize.x;
	Panels[NavigationPanel::PANEL_SATELLITES].Rect = NAS2D::Rectangle<int>::Create(panelPosition, panelSize);
	Panels[NavigationPanel::PANEL_SATELLITES].TextPosition = {Panels[NavigationPanel::PANEL_SATELLITES].Rect.x + panelSize.x / 2 - BIG_FONT->width(Panels[NavigationPanel::PANEL_SATELLITES].Name) / 2 + 20, text_y_position};
	Panels[NavigationPanel::PANEL_SATELLITES].IconPosition = {Panels[NavigationPanel::PANEL_SATELLITES].TextPosition.x - 40, 8};

	panelPosition.x += panelSize.x;
	Panels[NavigationPanel::PANEL_SPACEPORT].Rect = NAS2D::Rectangle<int>::Create(panelPosition, panelSize);
	Panels[NavigationPanel::PANEL_SPACEPORT].TextPosition = {Panels[NavigationPanel::PANEL_SPACEPORT].Rect.x + panelSize.x / 2 - BIG_FONT->width(Panels[NavigationPanel::PANEL_SPACEPORT].Name) / 2 + 20, text_y_position};
	Panels[NavigationPanel::PANEL_SPACEPORT].IconPosition = {Panels[NavigationPanel::PANEL_SPACEPORT].TextPosition.x - 40, 8};
}


/**
 * Draws a UI panel.
 */
static void drawPanel(Renderer& _r, Panel& _p)
{
	if (_p.Rect.contains(MOUSE_COORDS)) { _r.drawBoxFilled(_p.Rect, 0, 185, 185, 100); }

	if (_p.Selected())
	{
		_r.drawBoxFilled(_p.Rect, 0, 85, 0);

		if (_p.UiPanel) { _p.UiPanel->update(); }

		_r.drawText(*BIG_FONT_BOLD, _p.Name, _p.TextPosition, NAS2D::Color{185, 185, 0});
		_r.drawImage(*_p.Img, _p.IconPosition, 1.0f, NAS2D::Color{185, 185, 0});
	}
	else
	{
		_r.drawText(*BIG_FONT_BOLD, _p.Name, _p.TextPosition, NAS2D::Color{0, 185, 0});
		_r.drawImage(*_p.Img, _p.IconPosition, 1.0f, NAS2D::Color{0, 185, 0});
	}
}


/**
 * C'tor
 */
MainReportsUiState::MainReportsUiState()
{
	Utility<EventHandler>::get().windowResized().connect(this, &MainReportsUiState::onWindowResized);
	Utility<EventHandler>::get().keyDown().connect(this, &MainReportsUiState::onKeyDown);
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &MainReportsUiState::onMouseDown);
}


/**
 * D'tor
 */
MainReportsUiState::~MainReportsUiState()
{
	Utility<EventHandler>::get().windowResized().disconnect(this, &MainReportsUiState::onWindowResized);
	Utility<EventHandler>::get().keyDown().disconnect(this, &MainReportsUiState::onKeyDown);
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &MainReportsUiState::onMouseDown);

	delete WINDOW_BACKGROUND;

	for (Panel& panel : Panels)
	{
		delete panel.Img;
		delete panel.UiPanel;
	}
}


/**
 * Init
 */
void MainReportsUiState::initialize()
{
	WINDOW_BACKGROUND = new Image("ui/skin/window_middle_middle.png");

	BIG_FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, 16);
	BIG_FONT_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, 16);

	Panels[NavigationPanel::PANEL_EXIT].Img = new Image("ui/icons/exit.png");

	Panels[NavigationPanel::PANEL_RESEARCH].Img = new Image("ui/icons/research.png");
	Panels[NavigationPanel::PANEL_RESEARCH].Name = "Laboratories";

	Panels[NavigationPanel::PANEL_PRODUCTION].Img = new Image("ui/icons/production.png");
	Panels[NavigationPanel::PANEL_PRODUCTION].Name = "Factories";

	Panels[NavigationPanel::PANEL_WAREHOUSE].Img = new Image("ui/icons/warehouse.png");
	Panels[NavigationPanel::PANEL_WAREHOUSE].Name = "Warehouses";

	Panels[NavigationPanel::PANEL_MINING].Img = new Image("ui/icons/mine.png");
	Panels[NavigationPanel::PANEL_MINING].Name = "Mines";

	Panels[NavigationPanel::PANEL_SATELLITES].Img = new Image("ui/icons/satellite.png");
	Panels[NavigationPanel::PANEL_SATELLITES].Name = "Satellites";

	Panels[NavigationPanel::PANEL_SPACEPORT].Img = new Image("ui/icons/spaceport.png");
	Panels[NavigationPanel::PANEL_SPACEPORT].Name = "Space Ports";

	auto& renderer = Utility<Renderer>::get();
	const auto size = renderer.size().to<int>();
	setPanelRects(size.x);

	// INIT UI REPORT PANELS
	ReportInterface* factory_report = new FactoryReport();
	Panels[NavigationPanel::PANEL_PRODUCTION].UiPanel = factory_report;
	factory_report->position({0, 48});
	factory_report->size({size.x, size.y - 48});
	factory_report->hide();

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
	Panels[NavigationPanel::PANEL_PRODUCTION].UiPanel->fillLists();
	Panels[NavigationPanel::PANEL_PRODUCTION].UiPanel->refresh();

	Panels[NavigationPanel::PANEL_WAREHOUSE].UiPanel->fillLists();
	Panels[NavigationPanel::PANEL_WAREHOUSE].UiPanel->refresh();
}


/**
 * Wrapper deactivate handler.
 */
void MainReportsUiState::_deactivate()
{
	for (auto& panel : Panels)
	{
		if (panel.UiPanel) { panel.UiPanel->hide(); }
		panel.Selected(false);
	}

	Panels[NavigationPanel::PANEL_PRODUCTION].UiPanel->clearSelection();
	Panels[NavigationPanel::PANEL_WAREHOUSE].UiPanel->clearSelection();
}


/**
 * Key down event handler.
 */
void MainReportsUiState::onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier /*mod*/, bool /*repeat*/)
{
	if (!active()) { return; }
	if(key == NAS2D::EventHandler::KeyCode::KEY_ESCAPE) { exit(); }
}


/**
 * Mouse down event handler.
 */
void MainReportsUiState::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!active()) { return; }

	if (!NAS2D::Rectangle{0, 0, static_cast<int>(Utility<Renderer>::get().width()), 40}.contains(Point{x, y})) { return; } // ignore clicks in the UI area.

	if (button == EventHandler::MouseButton::BUTTON_LEFT)
	{
		for (Panel& panel : Panels)
		{
			bool selected = panel.Rect.contains(MOUSE_COORDS);
			panel.Selected(selected);

			if(panel.UiPanel) { panel.UiPanel->visible(selected); }
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
		if (panel.UiPanel) { panel.UiPanel->clearSelection(); }
	}

	mReportsUiCallback();
}


/**
 * Window resized event handler.
 */
void MainReportsUiState::onWindowResized(int w, int h)
{
	setPanelRects(w);
	for (Panel& panel : Panels)
	{
		if (panel.UiPanel) { panel.UiPanel->size(NAS2D::Vector{w, h - 48}); }
	}
}


/**
 * 
 */
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
void MainReportsUiState::selectMinePanel(Structure* /*structure*/)
{
	deselectAllPanels();
	Panels[NavigationPanel::PANEL_MINING].Selected(true);
	//Panels[NavigationPanel::PANEL_MINING].UiPanel->visible(true);
	//Panels[NavigationPanel::PANEL_MINING].UiPanel->refresh();
	//Panels[NavigationPanel::PANEL_MINING].UiPanel->selectStructure(structure);
}


void MainReportsUiState::clearLists()
{
	Panels[NavigationPanel::PANEL_PRODUCTION].UiPanel->fillLists();
	Panels[NavigationPanel::PANEL_WAREHOUSE].UiPanel->fillLists();
	//Panels[NavigationPanel::PANEL_MINING].UiPanel->fillLists();
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
		if (panel.UiPanel) { takeMeThereList.push_back(&panel.UiPanel->takeMeThereCallback()); }
	}
	return takeMeThereList;
}


/**
 * Draw
 */
State* MainReportsUiState::update()
{
	auto& renderer = Utility<Renderer>::get();

	renderer.clearScreen(NAS2D::Color{35, 35, 35});
	renderer.drawBoxFilled({0, 0, renderer.size().x, 48}, NAS2D::Color::Black);

	for (Panel& panel : Panels) { drawPanel(renderer, panel); }

	return this;
}
