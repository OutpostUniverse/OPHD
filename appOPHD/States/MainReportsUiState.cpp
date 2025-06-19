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
	constexpr auto ProductionPanelIndex = static_cast<size_t>(NavigationPanel::Production);
	constexpr auto WarehousePanelIndex = static_cast<size_t>(NavigationPanel::Warehouse);
	constexpr auto MinesPanelIndex = static_cast<size_t>(NavigationPanel::Mines);
	constexpr auto ExitPanelIndex = static_cast<size_t>(NavigationPanel::Exit);


	struct PanelInfo
	{
		ReportInterface* report{nullptr};
		const std::string name{};
		const NAS2D::Image* image{nullptr};
	};


	class Panel
	{
	public:
		void select(Structure* structure)
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

		void setMeta(const PanelInfo& panelInfo)
		{
			icon = panelInfo.image;
			name = panelInfo.name;
			report = panelInfo.report;
		}

	public:
		ReportInterface* report = nullptr;
		std::string name;
		const NAS2D::Image* icon = nullptr;

		NAS2D::Rectangle<int> tabArea;
		NAS2D::Point<int> textPosition;
		NAS2D::Point<int> iconPosition;

	private:
		bool mIsSelected = false;
	};


	static std::array<Panel, 7> panels;


	void setPanelRects(int width, const NAS2D::Font& font)
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


MainReportsUiState::MainReportsUiState() :
	fontMain{fontCache.load(constants::FontPrimaryBold, 16)}
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

	for (Panel& panel : panels)
	{
		delete panel.report;
	}
}


void MainReportsUiState::initialize()
{
	// INIT UI REPORT PANELS
	/* NOTE: Matches the order in enum NavigationPanel */
	auto panelInfo = std::array<PanelInfo, 7>{
		PanelInfo{new ResearchReport(), "Research", &imageCache.load("ui/icons/research.png")},
		PanelInfo{new FactoryReport(), "Factories", &imageCache.load("ui/icons/production.png")},
		PanelInfo{new WarehouseReport(), "Warehouses", &imageCache.load("ui/icons/warehouse.png")},
		PanelInfo{new MineReport(), "Mines", &imageCache.load("ui/icons/mine.png")},
		PanelInfo{new SatellitesReport(), "Satellites", &imageCache.load("ui/icons/satellite.png")},
		PanelInfo{new SpaceportsReport(), "Space Ports", &imageCache.load("ui/icons/spaceport.png")},
		PanelInfo{nullptr, "", &imageCache.load("ui/icons/exit.png")}
	};

	const auto size = NAS2D::Utility<NAS2D::Renderer>::get().size().to<int>();

	for (size_t i = 0; i < panelInfo.size(); i++)
	{
		auto& panel = panels[i];
		panel.setMeta(panelInfo[i]);
		auto* report = panel.report;
		if (report)
		{
			report->position({0, 48});
			report->size({size.x, size.y - 48});
			report->hide();
		}
	}

	setPanelRects(size.x, fontMain);
}


void MainReportsUiState::onActivate()
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


void MainReportsUiState::onDeactivate()
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


void MainReportsUiState::onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier /*mod*/, bool /*repeat*/)
{
	if (!active())
	{
		return;
	}

	if (key == NAS2D::KeyCode::Escape)
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
		exit();
	}
}


void MainReportsUiState::exit()
{
	deselectAllPanels();

	for (auto& panel : panels)
	{
		if (panel.report)
		{
			panel.report->clearSelected();
		}
	}

	mReportsUiSignal();
}


void MainReportsUiState::onWindowResized(NAS2D::Vector<int> newSize)
{
	setPanelRects(newSize.x, fontMain);
	for (Panel& panel : panels)
	{
		if (panel.report)
		{
			panel.report->area({{0, 48}, NAS2D::Vector{newSize.x, newSize.y - 48}});
		}
	}
}


void MainReportsUiState::deselectAllPanels()
{
	for (auto& panel : panels)
	{
		panel.selected(false);
	}
}


/**
 * Structure pointer is assumed to be a factory.
 */
void MainReportsUiState::selectFactoryPanel(Structure* structure)
{
	deselectAllPanels();
	panels[ProductionPanelIndex].select(structure);
}


/**
 * Structure pointer is assumed to be a warehouse.
 */
void MainReportsUiState::selectWarehousePanel(Structure* structure)
{
	deselectAllPanels();
	panels[WarehousePanelIndex].select(structure);
}


/**
 * Structure pointer is assumed to be a Mine Facility or Smelter.
 */
void MainReportsUiState::selectMinePanel(Structure* structure)
{
	deselectAllPanels();
	panels[MinesPanelIndex].select(structure);
}


void MainReportsUiState::injectTechnology(TechnologyCatalog& catalog, ResearchTracker& tracker)
{
	auto* researchPanel = panels[ResearchPanelIndex].report;
	dynamic_cast<ResearchReport&>(*researchPanel).injectTechReferences(catalog, tracker);
}


void MainReportsUiState::clearLists()
{
	for (auto& panel : panels)
	{
		if (panel.report)
		{
			panel.report->fillLists();
		}
	}
}


/**
 * Gets a list of TakeMeThere signal pointers.
 *
 * Acts as a pass-through for GameState.
 */
MainReportsUiState::TakeMeThereSignalSourceList MainReportsUiState::takeMeThere()
{
	TakeMeThereSignalSourceList takeMeThereList;
	for (auto& panel : panels)
	{
		if (panel.report)
		{
			takeMeThereList.push_back(&panel.report->takeMeThereSignal());
		}
	}

	return takeMeThereList;
}


NAS2D::State* MainReportsUiState::update()
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
