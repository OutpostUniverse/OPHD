#include "ResearchReport.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>

#include "../../Constants/UiConstants.h"
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

	constexpr auto LabTypeIconSize = 32;
	constexpr auto CategoryIconSize = 64;
	//constexpr auto TopicIconSize = 128; <-- Will be used in future change sets
	constexpr auto MarginSize = 10;

	constexpr NAS2D::Rectangle<int> HotLabIconRect = {{32, 224}, {LabTypeIconSize, LabTypeIconSize}};
	constexpr NAS2D::Rectangle<int> StandardLabIconRect = {{0, 224}, {LabTypeIconSize, LabTypeIconSize}};

	// Will be used in future change sets
	//constexpr NAS2D::Rectangle<int> TopicCompleteIconRect = {{0, 192}, {24, 24}};
	//constexpr NAS2D::Rectangle<int> TopicInProgressIconRect = {{24, 192}, {24, 24}};

	constexpr NAS2D::Vector<int> CategorySelectorPadding{2, 2};
	constexpr NAS2D::Vector<int> SectionPadding {10, 10};
}


ResearchReport::ResearchReport() :
	fontMedium{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryHuge)},
	imageLab{imageCache.load("ui/interface/lab_ug.png")},
	imageUiIcons{imageCache.load("ui/icons.png")},
	imageCategoryIcons{imageCache.load("categoryicons.png")},
	imageTopicIcons{imageCache.load("topicicons.png")},
	btnAllTopics{"All Topics", {100, LabTypeIconSize}, {this, &ResearchReport::onAllTopicsClicked}},
	btnAvailableTopics{"Available Topics", {100, LabTypeIconSize}, {this, &ResearchReport::onAvailableTopicsClicked}},
	btnCompletedTopics{"Completed Topics", {100, LabTypeIconSize}, {this, &ResearchReport::onCompletedTopicsClicked}},
	btnStandardLab{"Standard Lab", {100, LabTypeIconSize}, {this, &ResearchReport::onStandardLabClicked}},
	btnHotLab{"Hot Lab", {100, LabTypeIconSize}, {this, &ResearchReport::onHotLabClicked}}
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().connect({this, &ResearchReport::onMouseDown});

	const auto buttons = std::array{&btnAllTopics, &btnAvailableTopics, &btnCompletedTopics, &btnStandardLab, &btnHotLab};
	for (auto button : buttons)
	{
		add(*button, {});
		button->type(Button::Type::Toggle);
		button->toggle(false);
	}

	add(lstResearchTopics, {});
	lstResearchTopics.selectionChanged().connect({this, &ResearchReport::handleTopicChanged});

	const Point<int> buttonStartPosition{rect().position.x + MarginSize * 3 + CategoryIconSize, rect().position.y + MarginSize * 2 + fontBigBold.height()};
	const int buttonSpacing = btnAllTopics.size().x + MarginSize;

	for (size_t i = 0; i < buttons.size(); ++i)
	{
		buttons[i]->position(buttonStartPosition + Vector<int>{buttonSpacing * static_cast<int>(i), 0});
	}
}

ResearchReport::~ResearchReport()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().disconnect({this, &ResearchReport::onMouseDown});
}


void ResearchReport::fillLists()
{
	lstResearchTopics.clear();
	resetCategorySelection();
	onAllTopicsClicked();
}


void ResearchReport::clearSelected()
{
	lstResearchTopics.clearSelected();
	resetCategorySelection();
	onAllTopicsClicked();
}


void ResearchReport::refresh()
{
	if (mCategoryPanels.size() < 1) { return; }

	adjustCategoryIconSpacing();

	resetCategorySelection();
	onAllTopicsClicked();

	setSectionRects();

	lstResearchTopics.area(mResearchTopicArea);
}


void ResearchReport::setSectionRects()
{
	mCategoryIconArea =
	{
		mCategoryPanels.begin()->rect.startPoint(),
		{
			CategoryIconSize,
			mCategoryPanels.rbegin()->rect.endPoint().y - mCategoryPanels.begin()->rect.startPoint().y
		}
	};

	mResearchTopicArea =
	{
		{
			rect().position.x + MarginSize * 3 + CategoryIconSize,
			rect().position.y + fontBigBold.height() + btnAllTopics.size().y + MarginSize * 3
		},

		{
			((rect().size.x / 3) * 2) - (MarginSize * 4) - CategoryIconSize,
			rect().size.y - MarginSize * 4 - fontBigBold.height() - btnAllTopics.size().y
		}
	};
}


void ResearchReport::adjustCategoryIconSpacing()
{
	const int minimumHeight = CategoryIconSize * (static_cast<int>(mCategoryPanels.size()));
	const int padding = ((rect().size.y - 20) - minimumHeight) / static_cast<int>(mCategoryPanels.size() - 1);

	for (size_t i = 0; i < mCategoryPanels.size(); ++i)
	{
		const NAS2D::Point<int> point{rect().position.x + 10, rect().position.y + 10 + static_cast<int>(i) * CategoryIconSize + static_cast<int>(i) * padding};
		mCategoryPanels[i].rect = {point, {CategoryIconSize, CategoryIconSize}};
	}
}


void ResearchReport::resetCategorySelection()
{
	for (auto& panel : mCategoryPanels)
	{
		panel.selected = false;
	}

	mCategoryPanels.front().selected = true;
	mSelectedCategory = &mCategoryPanels.front();
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
		mCategoryPanels.emplace_back(CategoryPanel{
			{{0, 0}, {CategoryIconSize, CategoryIconSize}},
			{{(category.icon_index % columns) * CategoryIconSize, (category.icon_index / columns) * CategoryIconSize}, {CategoryIconSize, CategoryIconSize}},
			category.name,
			false});
	}

	std::sort(mCategoryPanels.begin(), mCategoryPanels.end(), [](const auto& a, const auto& b) { return a.name < b.name; });
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

	if (mCategoryIconArea.contains(position))
	{
		handleMouseDownInCategories(position);
	}
}


void ResearchReport::handleMouseDownInCategories(NAS2D::Point<int>& position)
{
	CategoryPanel* lastPanel = mSelectedCategory;
	bool panelClickedOn = false;
	for (auto& panel : mCategoryPanels)
	{
		panel.selected = false;

		if (panel.rect.contains(position))
		{
			panel.selected = true;
			mSelectedCategory = &panel;
			panelClickedOn = true;
		}
	}

	if (!panelClickedOn && lastPanel != nullptr)
	{
		mSelectedCategory = lastPanel;
		mSelectedCategory->selected = true;
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

	for (const auto& panel : mCategoryPanels)
	{
		const auto panelRect = Rectangle<int>::Create(
			panel.rect.position - CategorySelectorPadding,
			panel.rect.endPoint() + CategorySelectorPadding);

		if (panel.selected)
		{
			renderer.drawBoxFilled(panelRect, ColorPanelSelected);
		}
		else if (panel.rect.contains(MOUSE_COORDS))
		{
			renderer.drawBoxFilled(panelRect, ColorPanelHighlight);
		}

		renderer.drawSubImage(imageCategoryIcons, panel.rect.position, panel.imageSlice);
	}
}


void ResearchReport::drawTopicHeader() const
{
	auto& renderer = Utility<Renderer>::get();
	renderer.drawText(
		fontBigBold,
		mSelectedCategory->name,
		rect().position + Vector<int>{SectionPadding.x * 3 + CategoryIconSize, SectionPadding.y},
		ColorText);
}


void ResearchReport::drawVerticalSectionSpacer(const int startX) const
{
	auto& renderer = Utility<Renderer>::get();

	const Point<int> start{startX, rect().position.y + SectionPadding.y};
	const Point<int> end{startX, rect().position.y + rect().size.y - SectionPadding.y};
	renderer.drawLine(start, end, ColorText);
}


void ResearchReport::drawResearchPointsPanel() const
{
	auto& renderer = Utility<Renderer>::get();

	const auto startPoint = rect().position + Vector<int>{SectionPadding.x * 5 + CategoryIconSize + mResearchTopicArea.size.x, SectionPadding.y};

	renderer.drawText(fontBigBold, "Research Generated Per Turn", startPoint, ColorText);

	const auto standardLabStartPoint{startPoint + Vector<int>{0, fontBigBold.height() + SectionPadding.y}};
	const auto hotLabStartPoint{startPoint + Vector<int>{(rect().size.x - startPoint.x) / 2, fontBigBold.height() + SectionPadding.y}};

	renderer.drawSubImage(imageUiIcons, standardLabStartPoint, StandardLabIconRect);
	renderer.drawSubImage(imageUiIcons, hotLabStartPoint, HotLabIconRect);

	const auto standardLabTextOffset{standardLabStartPoint + Vector<int>{LabTypeIconSize + SectionPadding.x, LabTypeIconSize / 2 - fontMedium.height() / 2}};
	const auto hotLabTextOffset{hotLabStartPoint + Vector<int>{LabTypeIconSize + SectionPadding.x, LabTypeIconSize / 2 - fontMedium.height() / 2}};

	renderer.drawText(fontMedium, "0", standardLabTextOffset, ColorText);
	renderer.drawText(fontMedium, "0", hotLabTextOffset, ColorText);

	const Point<int> lineStartPoint{startPoint.x, rect().position.y + fontBigBold.height() + LabTypeIconSize + SectionPadding.y * 3};
	renderer.drawLine(lineStartPoint, lineStartPoint + Vector<int>{rect().size.x - startPoint.x - SectionPadding.x, 0}, ColorText);
}


void ResearchReport::handleTopicChanged()
{
}


void ResearchReport::draw() const
{
	drawCategories();

	drawVerticalSectionSpacer(mCategoryPanels.front().rect.endPoint().x + SectionPadding.x);

	drawTopicHeader();

	drawVerticalSectionSpacer((rect().size.x / 3) * 2);

	drawResearchPointsPanel();

}
