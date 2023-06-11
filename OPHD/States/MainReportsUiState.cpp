#include "MainReportsUiState.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"

#include "../UI/Reports/ReportInterface.h"

#include "../UI/Reports/FactoryReport.h"
#include "../UI/Reports/MineReport.h"
#include "../UI/Reports/ResearchReport.h"
#include "../UI/Reports/SatellitesReport.h"
#include "../UI/Reports/SpaceportsReport.h"
#include "../UI/Reports/WarehouseReport.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <array>


extern NAS2D::Point<int> MOUSE_COORDS;

namespace
{
	struct PanelInfo
	{
		ReportInterface* report{nullptr};
		const NAS2D::Image* image{nullptr};
		const std::string name{};
	};


	/**
	 * Enumerated IDs for the navigation panels.
	 */
	enum NavigationPanel
	{
		Research,
		Production,
		Warehouse,
		Mines,
		Satellites,
		Spaceports,
		Exit
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


	static std::array<Panel, 7> Panels; /**< Array of UI navigation panels. */


	/**
	 * Computes the panel rectangles for the top nav bar.
	 */
	void setPanelRects(int width, const NAS2D::Font& font)
	{
		Panels[NavigationPanel::Exit].Rect = {{width - 48, 0}, {48, 48}};
		Panels[NavigationPanel::Exit].IconPosition = {width - 40, 8};

		int remaining_width = width - Panels[NavigationPanel::Exit].Rect.size.x;
		const auto panelSize = NAS2D::Vector{remaining_width / 6, 48};

		auto panelPosition = NAS2D::Point{0, 0};
		for (std::size_t i = 0; i < NavigationPanel::Exit; ++i)
		{
			auto& panel = Panels[i];
			panel.Rect = NAS2D::Rectangle{panelPosition, panelSize};
			panel.TextPosition = panelPosition + (panelSize - font.size(panel.Name)) / 2 + NAS2D::Vector{20, 0};
			panel.IconPosition = {panel.TextPosition.x - 40, 8};
			panelPosition.x += panelSize.x;
		}
	}


	/**
	 * Draws a UI panel.
	 */
	void drawPanel(NAS2D::Renderer& renderer, Panel& panel, const NAS2D::Font& font)
	{
		if (panel.Rect.contains(MOUSE_COORDS)) { renderer.drawBoxFilled(panel.Rect, NAS2D::Color{0, 185, 185, 100}); }

		if (panel.Selected())
		{
			renderer.drawBoxFilled(panel.Rect, NAS2D::Color{0, 85, 0});

			if (panel.UiPanel) { panel.UiPanel->update(); }

			renderer.drawText(font, panel.Name, panel.TextPosition, NAS2D::Color{185, 185, 0});
			renderer.drawImage(*panel.Img, panel.IconPosition, 1.0f, NAS2D::Color{185, 185, 0});
		}
		else
		{
			renderer.drawText(font, panel.Name, panel.TextPosition, NAS2D::Color{0, 185, 0});
			renderer.drawImage(*panel.Img, panel.IconPosition, 1.0f, NAS2D::Color{0, 185, 0});
		}
	}
}



MainReportsUiState::MainReportsUiState() :
	fontMain{fontCache.load(constants::FONT_PRIMARY_BOLD, 16)}
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
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto size = renderer.size().to<int>();

	// INIT UI REPORT PANELS
	/* NOTE: Matches the order in enum::NavigationPanel */
	auto panelInfo = std::array<PanelInfo, 7>{
		PanelInfo{new ResearchReport(), &imageCache.load("ui/icons/research.png"), "Research"},
		PanelInfo{new FactoryReport(), &imageCache.load("ui/icons/production.png"), "Factories"},
		PanelInfo{new WarehouseReport(), &imageCache.load("ui/icons/warehouse.png"), "Warehouses"},
		PanelInfo{new MineReport(), &imageCache.load("ui/icons/mine.png"), "Mines"},
		PanelInfo{new SatellitesReport(), &imageCache.load("ui/icons/satellite.png"), "Satellites"},
		PanelInfo{new SpaceportsReport(), &imageCache.load("ui/icons/spaceport.png"), "Space Ports"},
		PanelInfo{nullptr, &imageCache.load("ui/icons/exit.png"), ""}
	};

	for (size_t i = 0; i < panelInfo.size(); i++)
	{
		auto& panel = Panels[i];
		panel.Img = panelInfo[i].image;
		panel.Name = panelInfo[i].name;
		panel.UiPanel = panelInfo[i].report;

		auto report = panel.UiPanel;
		if (report)
		{
			report->position({0, 48});
			report->size({size.x, size.y - 48});
			report->hide();
		}
	}

	setPanelRects(size.x, fontMain);
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
			panel.UiPanel->show();
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

	if (!NAS2D::Rectangle<int>{{0, 0}, {NAS2D::Utility<NAS2D::Renderer>::get().size().x, 40}}.contains(position)) { return; } // ignore clicks in the UI area.

	if (button == NAS2D::EventHandler::MouseButton::Left)
	{
		for (Panel& panel : Panels)
		{
			bool selected = panel.Rect.contains(MOUSE_COORDS);
			panel.Selected(selected);

			if (panel.UiPanel) { panel.UiPanel->visible(selected); }
		}
	}

	if (Panels[NavigationPanel::Exit].Selected())
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
	setPanelRects(newSize.x, fontMain);
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
	Panels[NavigationPanel::Production].Selected(true);
	Panels[NavigationPanel::Production].UiPanel->visible(true);

	Panels[NavigationPanel::Production].UiPanel->refresh();
	Panels[NavigationPanel::Production].UiPanel->selectStructure(structure);
}


/**
 * Structure pointer is assumed to be a warehouse.
 */
void MainReportsUiState::selectWarehousePanel(Structure* structure)
{
	deselectAllPanels();
	Panels[NavigationPanel::Warehouse].Selected(true);
	Panels[NavigationPanel::Warehouse].UiPanel->visible(true);
	Panels[NavigationPanel::Warehouse].UiPanel->refresh();
	Panels[NavigationPanel::Warehouse].UiPanel->selectStructure(structure);
}


/**
 * Structure pointer is assumed to be a Mine Facility or Smelter.
 */
void MainReportsUiState::selectMinePanel(Structure* structure)
{
	deselectAllPanels();
	Panels[NavigationPanel::Mines].Selected(true);
	Panels[NavigationPanel::Mines].UiPanel->visible(true);
	Panels[NavigationPanel::Mines].UiPanel->refresh();
	Panels[NavigationPanel::Mines].UiPanel->selectStructure(structure);
}


void MainReportsUiState::injectTechnology(TechnologyCatalog& catalog, ResearchTracker& tracker)
{
    auto researchPanel = Panels[NavigationPanel::Research].UiPanel;
    static_cast<ResearchReport*>(researchPanel)->injectTechReferences(catalog, tracker);
}


void MainReportsUiState::clearLists()
{
	Panels[NavigationPanel::Production].UiPanel->fillLists();
	Panels[NavigationPanel::Warehouse].UiPanel->fillLists();
	Panels[NavigationPanel::Mines].UiPanel->fillLists();
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
	renderer.drawBoxFilled(NAS2D::Rectangle<int>{{0, 0}, {renderer.size().x, 48}}, NAS2D::Color::Black);

	for (Panel& panel : Panels) { drawPanel(renderer, panel, fontMain); }

	return this;
}
