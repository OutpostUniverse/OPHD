// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MainReportsUiState.h"

#include "../Constants.h"
#include "../FontManager.h"
#include "../UI/Core/UIContainer.h"
#include "../UI/Reports/FactoryReport.h"


#include <array>

using namespace NAS2D;

extern Point_2d		MOUSE_COORDS;

static Image*		WINDOW_BACKGROUND = nullptr;

Font*				BIG_FONT = nullptr;
Font*				BIG_FONT_BOLD = nullptr;


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
 * 
 * \todo	This could be expanded to include a UI container which is where
 *			all UI elements can be housed.
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
	std::string		Name;

	Image*			Img = nullptr;

	Point_2d		TextPosition;
	Point_2d		IconPosition;

	Rectangle_2d	Rect;

	UIContainer*	UiPanel = nullptr;

private:
	bool			_selected = false;

};


static std::array<Panel, PANEL_COUNT> Panels;	/**< Array of UI navigation panels. */


/**
 * Computes the panel rectangles for the top nav bar.
 */
static void setPanelRects(int width, int height)
{
	Panels[PANEL_EXIT].Rect(width - 48, 0, 48, 48);
	Panels[PANEL_EXIT].IconPosition(width - 40, 8);
	
	int remaining_width = width - Panels[PANEL_EXIT].Rect.width();
	int panel_width = remaining_width / 6;
	int text_y_position = 24 - BIG_FONT->height() / 2;

	
	Panels[PANEL_RESEARCH].Rect(0, 0, panel_width, 48);
	Panels[PANEL_RESEARCH].TextPosition(panel_width / 2 - BIG_FONT->width(Panels[PANEL_RESEARCH].Name) / 2 + 20, text_y_position);
	Panels[PANEL_RESEARCH].IconPosition(Panels[PANEL_RESEARCH].TextPosition.x() - 40, 8);

	
	Panels[PANEL_PRODUCTION].Rect(Panels[PANEL_RESEARCH].Rect.x() + panel_width, 0, panel_width, 48);
	Panels[PANEL_PRODUCTION].TextPosition(Panels[PANEL_PRODUCTION].Rect.x() + panel_width / 2 - BIG_FONT->width(Panels[PANEL_PRODUCTION].Name) / 2 + 20, text_y_position);
	Panels[PANEL_PRODUCTION].IconPosition(Panels[PANEL_PRODUCTION].TextPosition.x() - 40, 8);
	//Panels[PANEL_PRODUCTION].Selected(true);


	Panels[PANEL_WAREHOUSE].Rect(Panels[PANEL_PRODUCTION].Rect.x() + panel_width, 0, panel_width, 48);
	Panels[PANEL_WAREHOUSE].TextPosition(Panels[PANEL_WAREHOUSE].Rect.x() + panel_width / 2 - BIG_FONT->width(Panels[PANEL_WAREHOUSE].Name) / 2 + 20, text_y_position);
	Panels[PANEL_WAREHOUSE].IconPosition(Panels[PANEL_WAREHOUSE].TextPosition.x() - 40, 8);


	Panels[PANEL_MINING].Rect(Panels[PANEL_WAREHOUSE].Rect.x() + panel_width, 0, panel_width, 48);
	Panels[PANEL_MINING].TextPosition(Panels[PANEL_MINING].Rect.x() + panel_width / 2 - BIG_FONT->width(Panels[PANEL_MINING].Name) / 2 + 20, text_y_position);
	Panels[PANEL_MINING].IconPosition(Panels[PANEL_MINING].TextPosition.x() - 40, 8);

	
	Panels[PANEL_SATELLITES].Rect(Panels[PANEL_MINING].Rect.x() + panel_width, 0, panel_width, 48);
	Panels[PANEL_SATELLITES].TextPosition(Panels[PANEL_SATELLITES].Rect.x() + panel_width / 2 - BIG_FONT->width(Panels[PANEL_SATELLITES].Name) / 2 + 20, text_y_position);
	Panels[PANEL_SATELLITES].IconPosition(Panels[PANEL_SATELLITES].TextPosition.x() - 40, 8);

	
	Panels[PANEL_SPACEPORT].Rect(Panels[PANEL_SATELLITES].Rect.x() + panel_width, 0, panel_width, 48);
	Panels[PANEL_SPACEPORT].TextPosition(Panels[PANEL_SPACEPORT].Rect.x() + panel_width / 2 - BIG_FONT->width(Panels[PANEL_SPACEPORT].Name) / 2 + 20, text_y_position);
	Panels[PANEL_SPACEPORT].IconPosition(Panels[PANEL_SPACEPORT].TextPosition.x() - 40, 8);
}


/**
 * Draws a UI panel.
 */
static void drawPanel(Renderer& _r, Panel& _p)
{
	if (isPointInRect(MOUSE_COORDS, _p.Rect)) { _r.drawBoxFilled(_p.Rect, 0, 185, 185, 100); }

	if (_p.Selected())
	{
		_r.drawBoxFilled(_p.Rect, 0, 85, 0);

		if (_p.UiPanel) { _p.UiPanel->update(); }

		_r.drawText(*BIG_FONT_BOLD, _p.Name, _p.TextPosition.x(), _p.TextPosition.y(), 185, 185, 0);
		_r.drawImage(*_p.Img, _p.IconPosition.x(), _p.IconPosition.y(), 1.0f, 185, 185, 0, 255);
	}
	else
	{
		_r.drawText(*BIG_FONT_BOLD, _p.Name, _p.TextPosition.x(), _p.TextPosition.y(), 0, 185, 0);
		_r.drawImage(*_p.Img, _p.IconPosition.x(), _p.IconPosition.y(), 1.0f, 0, 185, 0, 255);
	}
}



/**
 * C'tor
 */
MainReportsUiState::MainReportsUiState()
{
	Utility<EventHandler>::get().windowResized().connect(this, &MainReportsUiState::onWindowResized);
}


/**
 * D'tor
 */
MainReportsUiState::~MainReportsUiState()
{
	Utility<EventHandler>::get().windowResized().disconnect(this, &MainReportsUiState::onWindowResized);
	
	_deactivate();

	delete WINDOW_BACKGROUND;

	for (Panel& panel : Panels)
	{
		if (panel.Img) { delete panel.Img; panel.Img = nullptr; }
		if (panel.UiPanel) { delete panel.UiPanel; panel.UiPanel = nullptr; }
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

	Panels[PANEL_EXIT].Img = new Image("ui/icons/exit.png");

	Panels[PANEL_RESEARCH].Img = new Image("ui/icons/research.png");
	Panels[PANEL_RESEARCH].Name = "Laboratories";

	Panels[PANEL_PRODUCTION].Img = new Image("ui/icons/production.png");
	Panels[PANEL_PRODUCTION].Name = "Factories";

	Panels[PANEL_WAREHOUSE].Img = new Image("ui/icons/warehouse.png");
	Panels[PANEL_WAREHOUSE].Name = "Warehouses";

	Panels[PANEL_MINING].Img = new Image("ui/icons/mine.png");
	Panels[PANEL_MINING].Name = "Mines";

	Panels[PANEL_SATELLITES].Img = new Image("ui/icons/satellite.png");
	Panels[PANEL_SATELLITES].Name = "Satellites";

	Panels[PANEL_SPACEPORT].Img = new Image("ui/icons/spaceport.png");
	Panels[PANEL_SPACEPORT].Name = "Space Ports";

	Renderer& r = Utility<Renderer>::get();
	setPanelRects(r.width(), r.height());

	UIContainer* factory_report = new FactoryReport();
	Panels[PANEL_PRODUCTION].UiPanel = factory_report;
	factory_report->position(0, 48);
	factory_report->size(r.width(), r.height() - 48);
}


/**
 * Wrapper activate handler.
 */
void MainReportsUiState::_activate()
{
	Utility<EventHandler>::get().keyDown().connect(this, &MainReportsUiState::onKeyDown);
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &MainReportsUiState::onMouseDown);
	mReturnState = this;
}


/**
 * Wrapper deactivate handler.
 */
void MainReportsUiState::_deactivate()
{
	Utility<EventHandler>::get().keyDown().disconnect(this, &MainReportsUiState::onKeyDown);
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &MainReportsUiState::onMouseDown);
	//Utility<EventHandler>::get().windowResized().disconnect(this, &MainReportsUiState::onWindowResized);

	static_cast<FactoryReport*>(Panels[PANEL_PRODUCTION].UiPanel)->clearSelection();
}


/**
 * Key down event handler.
 */
void MainReportsUiState::onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool repeat)
{
	if (key == EventHandler::KEY_ESCAPE) { mReturnState = nullptr; }
}


/**
 * Mouse down event handler.
 */
void MainReportsUiState::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!isPointInRect(x, y, 0, 0, Utility<Renderer>::get().width(), 40)) { return; } // ignore clicks in the UI area.

	if (button == EventHandler::BUTTON_LEFT)
	{
		for (Panel& panel : Panels)
		{
			bool selected = isPointInRect(MOUSE_COORDS, panel.Rect);
			panel.Selected(selected);

			if(panel.UiPanel) { panel.UiPanel->visible(selected); }
		}
	}

	if (Panels[PANEL_EXIT].Selected())
	{
		mReturnState = nullptr;
		Panels[PANEL_EXIT].Selected(false);
	}
}


/**
 * Window resized event handler.
 */
void MainReportsUiState::onWindowResized(int w, int h)
{
	setPanelRects(w, h);
	for (Panel& panel : Panels) { if (panel.UiPanel) { panel.UiPanel->size(w, h - 48); } }
}


/**
 * 
 */
void MainReportsUiState::deselectAllPanels()
{
	for (auto& panel : Panels) { panel.Selected(false); }
}


/**
 * Structure pointer is assumed to be a factory.
 */
void MainReportsUiState::selectFactoryPanel(Structure* f)
{
	deselectAllPanels();
	Panels[PANEL_PRODUCTION].Selected(true);
	Panels[PANEL_PRODUCTION].UiPanel->visible(true);

	static_cast<FactoryReport*>(Panels[PANEL_PRODUCTION].UiPanel)->showAll();
	static_cast<FactoryReport*>(Panels[PANEL_PRODUCTION].UiPanel)->selectFactory(static_cast<Factory*>(f));
}


/**
 * Draw
 */
State* MainReportsUiState::update()
{
	Renderer& r = Utility<Renderer>::get();

	r.clearScreen(35, 35, 35);
	r.drawBoxFilled(0, 0, r.width(), 48, 0, 0, 0);

	for (Panel& panel : Panels) { drawPanel(r, panel); }

	return mReturnState;
}
