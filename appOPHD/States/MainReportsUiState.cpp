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

#include <NAS2D/EnumKeyCode.h>
#include <NAS2D/EnumMouseButton.h>
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


	class Panel
	{
	public:
		void Selected(bool isSelected)
		{
			mIsSelected = isSelected;
			if (UiPanel)
			{
				UiPanel->enabled(isSelected);
			}
		}

		bool Selected() const
		{
			return mIsSelected;
		}

		void setMeta(const PanelInfo& panelInfo)
		{
			Img = panelInfo.image;
			Name = panelInfo.name;
			UiPanel = panelInfo.report;
		}

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


	static std::array<Panel, 7> Panels;

	constexpr auto ExitPanelIndex = static_cast<size_t>(NavigationPanel::Exit);


	void setPanelRects(int width, const NAS2D::Font& font)
	{
		auto& exitPanel = Panels[ExitPanelIndex];
		exitPanel.Rect = {{width - 48, 0}, {48, 48}};
		exitPanel.IconPosition = {width - 40, 8};

		int remaining_width = width - exitPanel.Rect.size.x;
		const auto panelSize = NAS2D::Vector{remaining_width / 6, 48};

		auto panelPosition = NAS2D::Point{0, 0};
		for (std::size_t i = 0; i < Panels.size() - 1; ++i)
		{
			auto& panel = Panels[i];
			panel.Rect = NAS2D::Rectangle{panelPosition, panelSize};
			panel.TextPosition = panelPosition + (panelSize - font.size(panel.Name)) / 2 + NAS2D::Vector{20, 0};
			panel.IconPosition = {panel.TextPosition.x - 40, 8};
			panelPosition.x += panelSize.x;
		}
	}


	void drawPanel(NAS2D::Renderer& renderer, Panel& panel, const NAS2D::Font& font)
	{
		if (panel.Rect.contains(MOUSE_COORDS))
		{
			renderer.drawBoxFilled(panel.Rect, constants::HighliteColor);
		}

		auto drawColor = panel.Selected() ? constants::PrimaryColor : constants::SecondaryColor;

		if (panel.Selected())
		{
			renderer.drawBoxFilled(panel.Rect, constants::PrimaryColorVariant);

			if (panel.UiPanel)
			{
				panel.UiPanel->update();
			}
		}

		renderer.drawText(font, panel.Name, panel.TextPosition, drawColor);
		renderer.drawImage(*panel.Img, panel.IconPosition, 1.0f, drawColor);
	}


	void selectPanel(Panel& panel, Structure* structure)
	{
		panel.Selected(true);
		panel.UiPanel->visible(true);
		panel.UiPanel->refresh();
		panel.UiPanel->selectStructure(structure);
	}


	void setReportValues(ReportInterface* report, const NAS2D::Vector<int>& size)
	{
		if (report)
		{
			report->position({0, 48});
			report->size({size.x, size.y - 48});
			report->hide();
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
		panel.setMeta(panelInfo[i]);
		setReportValues(panel.UiPanel, size);
	}

	setPanelRects(size.x, fontMain);
}


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


void MainReportsUiState::onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier /*mod*/, bool /*repeat*/)
{
	if (!active())
	{
		return;
	}

	if (key == NAS2D::KeyCode::KEY_ESCAPE)
	{
		exit();
	}
}


void MainReportsUiState::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
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
		for (Panel& panel : Panels)
		{
			bool selected = panel.Rect.contains(MOUSE_COORDS);
			panel.Selected(selected);

			if (panel.UiPanel)
			{
				panel.UiPanel->visible(selected);
			}
		}
	}

	if (Panels[ExitPanelIndex].Selected())
	{
		exit();
	}
}


void MainReportsUiState::exit()
{
	deselectAllPanels();

	for (auto& panel : Panels)
	{
		if (panel.UiPanel)
		{
			panel.UiPanel->clearSelected();
		}
	}

	mReportsUiSignal();
}


void MainReportsUiState::onWindowResized(NAS2D::Vector<int> newSize)
{
	setPanelRects(newSize.x, fontMain);
	for (Panel& panel : Panels)
	{
		if (panel.UiPanel)
		{
			panel.UiPanel->size(NAS2D::Vector{newSize.x, newSize.y - 48});
		}
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
	selectPanel(Panels[static_cast<size_t>(NavigationPanel::Production)], structure);
}


/**
 * Structure pointer is assumed to be a warehouse.
 */
void MainReportsUiState::selectWarehousePanel(Structure* structure)
{
	deselectAllPanels();
	selectPanel(Panels[static_cast<size_t>(NavigationPanel::Warehouse)], structure);
}


/**
 * Structure pointer is assumed to be a Mine Facility or Smelter.
 */
void MainReportsUiState::selectMinePanel(Structure* structure)
{
	deselectAllPanels();
	selectPanel(Panels[static_cast<size_t>(NavigationPanel::Mines)], structure);
}


void MainReportsUiState::injectTechnology(TechnologyCatalog& catalog, ResearchTracker& tracker)
{
	auto researchPanel = Panels[static_cast<size_t>(NavigationPanel::Research)].UiPanel;
	static_cast<ResearchReport*>(researchPanel)->injectTechReferences(catalog, tracker);
}


void MainReportsUiState::clearLists()
{
	for (auto& panel : Panels)
	{
		if (panel.UiPanel)
		{
			panel.UiPanel->fillLists();
		}
	}
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
		if (panel.UiPanel)
		{
			takeMeThereList.push_back(&panel.UiPanel->takeMeThereSignal());
		}
	}

	return takeMeThereList;
}


NAS2D::State* MainReportsUiState::update()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	renderer.clearScreen(NAS2D::Color{35, 35, 35});
	renderer.drawBoxFilled(NAS2D::Rectangle<int>{{0, 0}, {renderer.size().x, 48}}, NAS2D::Color::Black);

	for (Panel& panel : Panels)
	{
		drawPanel(renderer, panel, fontMain);
	}

	return this;
}
