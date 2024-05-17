#include "ResearchReport.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>

#include "../../Constants/Strings.h"
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


	std::vector<ListBoxItemText> availableTopics(const std::string& categoryName, const TechnologyCatalog& catalog, const ResearchTracker& tracker)
	{
		std::vector<ListBoxItemText> itemsToAdd;
		const auto& completedTopics = tracker.completedResearch();
		for (const auto& topic : catalog.technologiesInCategory(categoryName))
		{
			const auto it = std::find(completedTopics.begin(), completedTopics.end(), topic.id);
			if (it == completedTopics.end())
			{
				itemsToAdd.emplace_back(ListBoxItemText{topic.name, topic.id});
			}
		}

		return itemsToAdd;
	}
}


ResearchReport::ResearchReport() :
	fontMedium{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryHuge)},
	imageLab{imageCache.load("ui/interface/lab_ug.png")},
	imageUiIcons{imageCache.load("ui/icons.png")},
	imageCategoryIcons{imageCache.load("categoryicons.png")},
	imageTopicIcons{imageCache.load("topicicons.png")},
	txtTopicDescription{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryMedium)}
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().connect({this, &ResearchReport::onMouseDown});

	add(lstResearchTopics, {});
	lstResearchTopics.selectionChanged().connect({this, &ResearchReport::handleTopicChanged});

	add(txtTopicDescription, {});
}


ResearchReport::~ResearchReport()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().disconnect({this, &ResearchReport::onMouseDown});
}


void ResearchReport::fillLists()
{
	resetCategorySelection();
}


void ResearchReport::clearSelected()
{
	resetCategorySelection();
}


void ResearchReport::refresh()
{
	if (mCategoryPanels.empty()) { return; }

	adjustCategoryIconSpacing();

	resetCategorySelection();

	setSectionRects();

	lstResearchTopics.area(mResearchTopicArea);

	txtTopicDescription.text("");
	txtTopicDescription.area(mTopicDetailsArea);
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

	std::sort(mCategoryPanels.begin(), mCategoryPanels.end());
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
			resetResearchDetails();
		}
	}

	if (!panelClickedOn && lastPanel != nullptr)
	{
		mSelectedCategory = lastPanel;
		mSelectedCategory->selected = true;
	}
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
			rect().position.y + fontBigBold.height() + MarginSize * 3
		},

		{
			((rect().size.x / 3) * 2) - (MarginSize * 4) - CategoryIconSize,
			rect().size.y - MarginSize * 4 - fontBigBold.height()
		}
	};

	
	mTopicDetailsHeaderArea =
	{
		rect().position + Vector<int>{SectionPadding.x * 2 + mResearchTopicArea.endPoint().x, SectionPadding.y},
		{
			rect().size.x - mResearchTopicArea.endPoint().x - SectionPadding.x * 3,
			100
		}
	};
	
	mTopicDetailsArea =
	{
		mTopicDetailsHeaderArea.position + Vector<int>{0, mTopicDetailsHeaderArea.size.y + SectionPadding.x * 2},
		{
			mTopicDetailsHeaderArea.size.x,
			mCategoryIconArea.size.y - mTopicDetailsHeaderArea.size.y - SectionPadding.x * 2
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
	resetResearchDetails();
}


void ResearchReport::fillResearchTopicsList()
{
	std::vector<ListBoxItemText> itemsToAdd = availableTopics(mSelectedCategory->name, *mTechCatalog, *mResearchTracker);
	std::sort(itemsToAdd.begin(), itemsToAdd.end());
	
	for (auto& item : itemsToAdd)
	{
		lstResearchTopics.add(item);
	}
}


void ResearchReport::resetResearchDetails()
{
	lstResearchTopics.clear();
	txtTopicDescription.text("");

	fillResearchTopicsList();
}


void ResearchReport::handleTopicChanged()
{
	txtTopicDescription.text("");

	if (lstResearchTopics.selectedIndex() == ListBox<ListBoxItemText>::NoSelection)
	{
		return;
	}

	const auto& technology = mTechCatalog->technologyFromId(lstResearchTopics.selected().tag);
	txtTopicDescription.text(technology.description);
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


void ResearchReport::drawCategoryHeader() const
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


void ResearchReport::drawTopicHeaderPanel() const
{
	auto& renderer = Utility<Renderer>::get();

	const auto startPoint = mTopicDetailsHeaderArea.startPoint();

	renderer.drawText(fontBigBold, constants::ResearchReportTopicDetails, startPoint, ColorText);

	const auto standardLabStartPoint{startPoint + Vector<int>{0, fontBigBold.height() + SectionPadding.y}};
	const auto hotLabStartPoint{startPoint + Vector<int>{(rect().size.x - startPoint.x) / 2, fontBigBold.height() + SectionPadding.y}};

	renderer.drawSubImage(imageUiIcons, standardLabStartPoint, StandardLabIconRect);
	renderer.drawSubImage(imageUiIcons, hotLabStartPoint, HotLabIconRect);

	const auto standardLabTextOffset{standardLabStartPoint + Vector<int>{LabTypeIconSize + SectionPadding.x, LabTypeIconSize / 2 - fontMedium.height() / 2}};
	const auto hotLabTextOffset{hotLabStartPoint + Vector<int>{LabTypeIconSize + SectionPadding.x, LabTypeIconSize / 2 - fontMedium.height() / 2}};

	renderer.drawText(fontMedium, "0", standardLabTextOffset, ColorText);
	renderer.drawText(fontMedium, "0", hotLabTextOffset, ColorText);

	const Point<int> lineStartPoint{mTopicDetailsHeaderArea.crossYPoint() + Vector<int>{0, SectionPadding.y}};
	const Point<int> lineEndPoint{mTopicDetailsHeaderArea.endPoint() + Vector<int>{0, SectionPadding.y}};

	renderer.drawLine(lineStartPoint, lineEndPoint, ColorText);
}


void ResearchReport::draw() const
{
	drawCategories();

	drawVerticalSectionSpacer(mCategoryPanels.front().rect.endPoint().x + SectionPadding.x);

	drawCategoryHeader();

	drawVerticalSectionSpacer((rect().size.x / 3) * 2);

	drawTopicHeaderPanel();
}
