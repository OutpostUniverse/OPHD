#include "ResearchReport.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Rectangle.h>

#include "../../Cache.h"

#include <vector>

extern NAS2D::Point<int> MOUSE_COORDS;	// <-- Yuck, really need to find a better way to
										// poll mouse position. Might make sense to add a
										// method to NAS2D::EventHandler for that.

namespace
{
	constexpr NAS2D::Color ColorPanelHighlight{0, 185, 185, 100};
	constexpr NAS2D::Color ColorPanelSelected{0, 85, 0};

	constexpr auto CategoryIconSize = 64;
	constexpr auto TopicIconSize = 128;

	constexpr NAS2D::Rectangle<int> HotLabIconRect = {32, 224, 32, 32};
	constexpr NAS2D::Rectangle<int> StandardLabIconRect = {0, 224, 32, 32};

	constexpr NAS2D::Rectangle<int> TopicCompleteIconRect = {0, 192, 24, 24};
	constexpr NAS2D::Rectangle<int> TopicInProgressIconRect = {24, 192, 24, 24};

	struct CategoryPanel
	{
		NAS2D::Rectangle<int> rect{};
		NAS2D::Rectangle<int> imageSlice{};
		std::string name{};
		bool selected{false};
	};

	std::vector<CategoryPanel> CategoryPanels;

};


ResearchReport::ResearchReport() :
	fontMedium{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryHuge)},
	imageLab{imageCache.load("ui/interface/lab_ug.png")},
	imageUiIcons{imageCache.load("ui/icons.png")},
	imageCategoryIcons{imageCache.load("categoryicons.png")},
	imageTopicIcons{imageCache.load("topicicons.png")}
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().connect({this, &ResearchReport::onMouseDown});
}


ResearchReport::~ResearchReport()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().connect({this, &ResearchReport::onMouseDown});
}


void ResearchReport::fillLists()
{
}


void ResearchReport::clearSelected()
{
}


void ResearchReport::refresh()
{
	if (CategoryPanels.size() < 1) { return; }

	const int minimumHeight = CategoryIconSize * static_cast<int>(CategoryPanels.size());
	const int padding = ((rect().height - 20) - minimumHeight) / static_cast<int>(CategoryPanels.size() - 1);
	
	for (int i = 0; i < CategoryPanels.size(); ++i)
	{
		const NAS2D::Point<int> point{rect().x + 10, rect().y + 10 + i * CategoryIconSize + i * padding};
		CategoryPanels[i].rect = {point.x, point.y, CategoryIconSize, CategoryIconSize};
	}
	
	CategoryPanels.front().selected = true;
}


void ResearchReport::selectStructure(Structure*)
{
}


void ResearchReport::injectTechReferences(TechnologyCatalog& catalog, ResearchTracker& tracker)
{
    mTechCatalog = &catalog;
    mResearchTracker = &tracker;

	const int columns = imageCategoryIcons.size().x / CategoryIconSize;

	for (const auto& category : mTechCatalog->categories())
	{
		CategoryPanels.emplace_back(CategoryPanel{
			{0, 0, CategoryIconSize, CategoryIconSize},
			{(category.icon_index % columns) * CategoryIconSize, (category.icon_index / columns) * CategoryIconSize, CategoryIconSize, CategoryIconSize},
			category.name,
			false});
	}

	std::sort(CategoryPanels.begin(), CategoryPanels.end(), [](const auto& a, const auto& b) { return a.name < b.name; });
	refresh();
}


void ResearchReport::update()
{
	draw();
	UIContainer::update();
}


void ResearchReport::onResize()
{
	refresh();
}

void ResearchReport::onMouseDown(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position)
{
	if (button != NAS2D::EventHandler::MouseButton::Left) { return; }

	for (auto& panel : CategoryPanels)
	{
		panel.selected = panel.rect.contains(position) ? true : false;
	}
}


void ResearchReport::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	for (const auto& panel : CategoryPanels)
	{
		if (panel.selected)
		{
			renderer.drawBoxFilled(panel.rect, ColorPanelSelected);
		}
		else if (panel.rect.contains(MOUSE_COORDS))
		{
			renderer.drawBoxFilled(panel.rect, ColorPanelHighlight);
		}

		renderer.drawSubImage(imageCategoryIcons, panel.rect.startPoint(), panel.imageSlice);
	}
}