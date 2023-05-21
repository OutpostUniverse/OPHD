#include "ResearchReport.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Rectangle.h>

#include "../../Cache.h"

#include <array>
#include <vector>

extern NAS2D::Point<int> MOUSE_COORDS;	// <-- Yuck, really need to find a better way to
										// poll mouse position. Might make sense to add a
										// method to NAS2D::EventHandler for that.

using namespace NAS2D;

namespace
{
	constexpr NAS2D::Color ColorPanelHighlight{0, 185, 185, 100};
	constexpr NAS2D::Color ColorPanelSelected{0, 85, 0};
	constexpr NAS2D::Color ColorText{0, 185, 0};

	constexpr auto CategoryIconSize = 64;
	constexpr auto TopicIconSize = 128;
	constexpr auto MarginSize = 10;

	constexpr NAS2D::Rectangle<int> HotLabIconRect = {32, 224, 32, 32};
	constexpr NAS2D::Rectangle<int> StandardLabIconRect = {0, 224, 32, 32};

	constexpr NAS2D::Rectangle<int> TopicCompleteIconRect = {0, 192, 24, 24};
	constexpr NAS2D::Rectangle<int> TopicInProgressIconRect = {24, 192, 24, 24};

	constexpr NAS2D::Vector<int> CategorySelectorPadding{2, 2};
	constexpr NAS2D::Vector<int> SectionPadding {10, 10};

    struct CategoryPanel
    {
        NAS2D::Rectangle<int> rect{};
        NAS2D::Rectangle<int> imageSlice{};
        std::string name{};
        bool selected{false};
    };

    CategoryPanel* SelectedCategory{ nullptr };

	Rectangle<int> IconArea{};

	std::vector<CategoryPanel> CategoryPanels;
};


ResearchReport::ResearchReport() :
	fontMedium{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryHuge)},
	imageLab{imageCache.load("ui/interface/lab_ug.png")},
	imageUiIcons{imageCache.load("ui/icons.png")},
	imageCategoryIcons{imageCache.load("categoryicons.png")},
	imageTopicIcons{imageCache.load("topicicons.png")},
	btnAllTopics{"All Topics", {100, 32}, {this, &ResearchReport::onAllTopicsClicked}},
	btnAvailableTopics{"Available Topics", {100, 32}, {this, &ResearchReport::onAvailableTopicsClicked}},
	btnCompletedTopics{"Completed Topics", {100, 32}, {this, &ResearchReport::onCompletedTopicsClicked}},
	btnStandardLab{"Standard Lab", {100, 32}, {this, &ResearchReport::onStandardLabClicked}},
	btnHotLab{"Hot Lab", {100, 32}, {this, &ResearchReport::onHotLabClicked}}
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().connect({this, &ResearchReport::onMouseDown});

	const auto buttons = std::array{&btnAllTopics, &btnAvailableTopics, &btnCompletedTopics, &btnStandardLab, &btnHotLab};
	for (auto button : buttons)
	{
		add(*button, {});
		button->type(Button::Type::Toggle);
		button->toggle(false);
	}

	const Point<int> buttonStartPosition{rect().x + MarginSize * 3 + CategoryIconSize, rect().y + MarginSize * 2 + fontBigBold.height()};
	const int buttonSpacing = btnAllTopics.size().x + MarginSize;

	for (int i = 0; i < buttons.size(); ++i)
	{
		buttons[i]->position(buttonStartPosition + Vector<int>{buttonSpacing * i, 0});
	}
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

	const int minimumHeight = CategoryIconSize * (static_cast<int>(CategoryPanels.size()));
	const int padding = ((rect().height - 20) - minimumHeight) / static_cast<int>(CategoryPanels.size() - 1);
	
	for (int i = 0; i < CategoryPanels.size(); ++i)
	{
		const NAS2D::Point<int> point{rect().x + 10, rect().y + 10 + i * CategoryIconSize + i * padding};
		CategoryPanels[i].rect = {point.x, point.y, CategoryIconSize, CategoryIconSize};
	}
	
	CategoryPanels.front().selected = true;
    SelectedCategory = &CategoryPanels.front();

	onAllTopicsClicked();

	IconArea = {
		rect().x + MarginSize * 3 + CategoryIconSize,
		rect().y + fontBigBold.height() + btnAllTopics.size().y + MarginSize * 3,
		((rect().width / 3) * 2) - (MarginSize * 4) - CategoryIconSize,
		rect().height - MarginSize * 4 - fontBigBold.height() - btnAllTopics.size().y};
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
	if (!visible() || 
		!rect().contains(position) ||
		button != NAS2D::EventHandler::MouseButton::Left)
	{
		return;
	}
    
    CategoryPanel* lastPanel = SelectedCategory;
    bool panelClickedOn = false;
	for (auto& panel : CategoryPanels)
	{
        if(panel.rect.contains(position))
        {
            panel.selected = true;
            SelectedCategory = &panel;
            panelClickedOn = true;
        }
        else
        {
            panel.selected = false;
        }
	}
    
    if(!panelClickedOn && lastPanel != nullptr)
    {
        SelectedCategory = lastPanel;
        SelectedCategory->selected = true;
    }
}


void ResearchReport::untoggleAllButtons()
{
	btnAllTopics.toggle(false);
	btnAvailableTopics.toggle(false);
	btnCompletedTopics.toggle(false);
	btnStandardLab.toggle(false);
	btnHotLab.toggle(false);
}


void ResearchReport::onAllTopicsClicked()
{
	untoggleAllButtons();
	btnAllTopics.toggle(true);
}


void ResearchReport::onAvailableTopicsClicked()
{
	untoggleAllButtons();
	btnAvailableTopics.toggle(true);
}


void ResearchReport::onCompletedTopicsClicked()
{
	untoggleAllButtons();
	btnCompletedTopics.toggle(true);
}


void ResearchReport::onStandardLabClicked()
{
	untoggleAllButtons();
	btnStandardLab.toggle(true);
}


void ResearchReport::onHotLabClicked()
{
	untoggleAllButtons();
	btnHotLab.toggle(true);
}


void ResearchReport::drawCategories() const
{
	auto& renderer = Utility<Renderer>::get();

	for (const auto& panel : CategoryPanels)
	{
		const auto panelRect = Rectangle<int>::Create(
			panel.rect.startPoint() - CategorySelectorPadding,
			panel.rect.endPoint() + CategorySelectorPadding);

		if (panel.selected)
		{
			renderer.drawBoxFilled(panelRect, ColorPanelSelected);
		}
		else if (panel.rect.contains(MOUSE_COORDS))
		{
			renderer.drawBoxFilled(panelRect, ColorPanelHighlight);
		}

		renderer.drawSubImage(imageCategoryIcons, panel.rect.startPoint(), panel.imageSlice);
	}
}


void ResearchReport::drawTopicHeader() const
{
	auto& renderer = Utility<Renderer>::get();
	renderer.drawText(
		fontBigBold,
		SelectedCategory->name,
		rect().startPoint() + Vector<int>{SectionPadding.x * 3 + CategoryIconSize, SectionPadding.y},
		ColorText);
}


void ResearchReport::drawVerticalSectionSpacer(const int startX) const
{
	auto& renderer = Utility<Renderer>::get();
	renderer.drawLine(
		Point<int>{startX, rect().y + SectionPadding.y},
		Point<int>{startX, rect().y + rect().height - SectionPadding.y},
		ColorText);
}


void ResearchReport::drawTopicIconPanel() const
{
    auto& renderer = Utility<Renderer>::get();
    renderer.drawBox(IconArea, ColorText);
}


void ResearchReport::drawResearchPointsPanel() const
{
    auto& renderer = Utility<Renderer>::get();
    
    const auto startPoint = rect().startPoint() + Vector<int>{SectionPadding.x * 5 + CategoryIconSize + IconArea.width, SectionPadding.y};
    
    renderer.drawText(
      fontBigBold,
      "Research Generated Per Turn",
      startPoint,
      ColorText);
    
    renderer.drawSubImage(imageUiIcons, startPoint + Vector<int>{0, fontBigBold.height() + SectionPadding.y}, StandardLabIconRect);
    renderer.drawSubImage(imageUiIcons, startPoint + Vector<int>{(rect().width - startPoint.x) / 2, fontBigBold.height() + SectionPadding.y}, HotLabIconRect);
	
	const Point<int> lineStartPoint{startPoint.x, rect().y + fontBigBold.height() + HotLabIconRect.height + SectionPadding.y * 3};
	renderer.drawLine(lineStartPoint, lineStartPoint + Vector<int>{rect().width - startPoint.x - SectionPadding.x, 0}, ColorText);
}


void ResearchReport::draw() const
{
	drawCategories();

	drawVerticalSectionSpacer(CategoryPanels.front().rect.endPoint().x + SectionPadding.x);

	drawTopicHeader();
    drawTopicIconPanel();

	drawVerticalSectionSpacer((rect().width / 3) * 2);
    
    drawResearchPointsPanel();
    
}
