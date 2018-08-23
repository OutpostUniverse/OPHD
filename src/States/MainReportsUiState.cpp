// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MainReportsUiState.h"

#include <array>

using namespace NAS2D;

extern Point_2d			MOUSE_COORDS;


static Image*			WINDOW_BACKGROUND = nullptr;

static Font*			BIG_FONT = nullptr;
static Font*			BIG_FONT_BOLD = nullptr;

static bool				CAN_DRAW_ICON = false;


/**
 * 
 */
enum NavigationPanel
{
	PANEL_RESEARCH,
	PANEL_PRODUCTION,
	PANEL_MINING,
	PANEL_SATELLITES,
	PANEL_SPACEPORT,
	PANEL_EXIT,
	PANEL_COUNT
};


struct Panel
{
	Rectangle_2d	Rect;
	std::string		Name;
	Image*			Img = nullptr;
	Point_2d		TextPosition;
	Point_2d		IconPosition;
	bool			Selected = false;
};


static std::array<Panel, PANEL_COUNT> Panels;


/**
 * Computes the panel rectangles for the top nav bar.
 */
static void setPanelRects()
{
	Renderer& r = Utility<Renderer>::get();

	Panels[PANEL_EXIT].Rect(r.width() - 48, 0, 48, 48);
	Panels[PANEL_EXIT].IconPosition(r.width() - 40, 8);
	
	int remaining_width = r.width() - Panels[PANEL_EXIT].Rect.width();
	int panel_width = remaining_width / 5;
	int text_y_position = 24 - BIG_FONT->height() / 2;

	
	Panels[PANEL_RESEARCH].Rect(0, 0, panel_width, 48);
	Panels[PANEL_RESEARCH].TextPosition(panel_width / 2 - BIG_FONT->width(Panels[PANEL_RESEARCH].Name) / 2 + 20, text_y_position);
	Panels[PANEL_RESEARCH].IconPosition(Panels[PANEL_RESEARCH].TextPosition.x() - 40, 8);

	
	Panels[PANEL_PRODUCTION].Rect(Panels[PANEL_RESEARCH].Rect.x() + panel_width, 0, panel_width, 48);
	Panels[PANEL_PRODUCTION].TextPosition(Panels[PANEL_PRODUCTION].Rect.x() + panel_width / 2 - BIG_FONT->width(Panels[PANEL_PRODUCTION].Name) / 2 + 20, text_y_position);
	Panels[PANEL_PRODUCTION].IconPosition(Panels[PANEL_PRODUCTION].TextPosition.x() - 40, 8);


	Panels[PANEL_MINING].Rect(Panels[PANEL_PRODUCTION].Rect.x() + panel_width, 0, panel_width, 48);
	Panels[PANEL_MINING].TextPosition(Panels[PANEL_MINING].Rect.x() + panel_width / 2 - BIG_FONT->width(Panels[PANEL_MINING].Name) / 2 + 20, text_y_position);
	Panels[PANEL_MINING].IconPosition(Panels[PANEL_MINING].TextPosition.x() - 40, 8);

	
	Panels[PANEL_SATELLITES].Rect(Panels[PANEL_MINING].Rect.x() + panel_width, 0, panel_width, 48);
	Panels[PANEL_SATELLITES].TextPosition(Panels[PANEL_SATELLITES].Rect.x() + panel_width / 2 - BIG_FONT->width(Panels[PANEL_SATELLITES].Name) / 2 + 20, text_y_position);
	Panels[PANEL_SATELLITES].IconPosition(Panels[PANEL_SATELLITES].TextPosition.x() - 40, 8);

	
	Panels[PANEL_SPACEPORT].Rect(Panels[PANEL_SATELLITES].Rect.x() + panel_width, 0, panel_width, 48);
	Panels[PANEL_SPACEPORT].TextPosition(Panels[PANEL_SPACEPORT].Rect.x() + panel_width / 2 - BIG_FONT->width(Panels[PANEL_SPACEPORT].Name) / 2 + 20, text_y_position);
	Panels[PANEL_SPACEPORT].IconPosition(Panels[PANEL_SPACEPORT].TextPosition.x() - 40, 8);
}


static void drawPanel(Renderer& _r, Panel& _p)
{
	if (_p.Selected) { _r.drawBoxFilled(_p.Rect, 0, 85, 0); }
	_r.drawText(*BIG_FONT_BOLD, _p.Name, _p.TextPosition.x(), _p.TextPosition.y(), 0, 185, 0);
	_r.drawImage(*_p.Img, _p.IconPosition.x(), _p.IconPosition.y(), 1.0f, 0, 185, 0, 255);

	if (isPointInRect(MOUSE_COORDS, _p.Rect)) { _r.drawBoxFilled(_p.Rect, 0, 185, 185, 100); }

}



/**
 * C'tor
 */
MainReportsUiState::MainReportsUiState()
{}


/**
 * D'tor
 */
MainReportsUiState::~MainReportsUiState()
{
	delete WINDOW_BACKGROUND;
	delete BIG_FONT;
	delete BIG_FONT_BOLD;

	for (auto panel : Panels)
	{
		if (panel.Img) { delete panel.Img; }
	}
}


/**
 * Init
 */
void MainReportsUiState::initialize()
{
	WINDOW_BACKGROUND = new Image("ui/skin/window_middle_middle.png");

	BIG_FONT = new Font("fonts/opensans.ttf", 16);
	BIG_FONT_BOLD = new Font("fonts/opensans-bold.ttf", 16);

	Panels[PANEL_EXIT].Img = new Image("ui/icons/exit.png");

	Panels[PANEL_RESEARCH].Img = new Image("ui/icons/research.png");
	Panels[PANEL_RESEARCH].Name = "Laboratories";

	Panels[PANEL_PRODUCTION].Img = new Image("ui/icons/production.png");
	Panels[PANEL_PRODUCTION].Name = "Factories";

	Panels[PANEL_MINING].Img = new Image("ui/icons/mine.png");
	Panels[PANEL_MINING].Name = "Mines";

	Panels[PANEL_SATELLITES].Img = new Image("ui/icons/satellite.png");
	Panels[PANEL_SATELLITES].Name = "Satellites";

	Panels[PANEL_SPACEPORT].Img = new Image("ui/icons/spaceport.png");
	Panels[PANEL_SPACEPORT].Name = "Space Ports";

	setPanelRects();
}


/**
 * Wrapper activate handler.
 */
void MainReportsUiState::_activate()
{
	Utility<EventHandler>::get().keyDown().connect(this, &MainReportsUiState::onKeyDown);
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &MainReportsUiState::onMouseDown);
	Utility<EventHandler>::get().windowResized().connect(this, &MainReportsUiState::onWindowResized);
	mReturnState = this;
}


/**
 * Wrapper deactivate handler.
 */
void MainReportsUiState::_deactivate()
{
	Utility<EventHandler>::get().keyDown().disconnect(this, &MainReportsUiState::onKeyDown);
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &MainReportsUiState::onMouseDown);
	Utility<EventHandler>::get().windowResized().disconnect(this, &MainReportsUiState::onWindowResized);
}


/**
 * Key down event handler.
 */
void MainReportsUiState::onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool repeat)
{
	if (key == EventHandler::KEY_ESCAPE) { mReturnState = nullptr; }
}


void MainReportsUiState::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (button == EventHandler::BUTTON_LEFT)
	{
		for (Panel& panel : Panels)
		{
			if (isPointInRect(MOUSE_COORDS, panel.Rect))
			{
				panel.Selected = true;
			}
			else
			{
				panel.Selected = false;
			}
		}
	}
}


/**
 * Window resized event handler.
 */
void MainReportsUiState::onWindowResized(int w, int h)
{
	setPanelRects();
}


/**
 * Draw
 */
State* MainReportsUiState::update()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawImageRepeated(*WINDOW_BACKGROUND, 0, 0, r.width(), r.height());
	r.drawBoxFilled(0, 0, r.width(), 48, 0, 0, 0);

	
	for (auto panel : Panels) { drawPanel(r, panel); }
	

	//drawPanel(r, Panels[PANEL_RESEARCH]);

	return mReturnState;
}
