#include "ResearchReport.h"

#include "../../Constants/Strings.h"
#include "../../Constants/UiConstants.h"
#include "../../Cache.h"
#include "../../StructureManager.h"

#include "../../MapObjects/Structures/HotLaboratory.h"
#include "../../MapObjects/Structures/Laboratory.h"

#include <libOPHD/Technology/TechnologyCatalog.h>
#include <libOPHD/Technology/ResearchTracker.h>

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Renderer/Renderer.h>

#include <array>
#include <vector>
#include <algorithm>


extern NAS2D::Point<int> MOUSE_COORDS;	// <-- Yuck, really need to find a better way to
										// poll mouse position. Might make sense to add a
										// method to NAS2D::EventHandler for that.

using namespace NAS2D;

namespace
{
	constexpr Vector<int> LabTypeIconSize{32, 32};
	constexpr Vector<int> CategoryIconSize{64, 64};
	constexpr Vector<int> TopicIconSize{128, 128};
	constexpr auto MarginSize = 10;

	constexpr NAS2D::Rectangle<int> HotLabIconRect = {{32, 224}, LabTypeIconSize};
	constexpr NAS2D::Rectangle<int> StandardLabIconRect = {{0, 224}, LabTypeIconSize};

	// Will be used in future change sets
	//constexpr NAS2D::Rectangle<int> TopicCompleteIconRect = {{0, 192}, {24, 24}};
	//constexpr NAS2D::Rectangle<int> TopicInProgressIconRect = {{24, 192}, {24, 24}};

	constexpr NAS2D::Vector<int> CategorySelectorPadding{2, 2};
	constexpr NAS2D::Vector<int> SectionPadding{10, 10};

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

	void drawDetailsHeaderSeparator(const Rectangle<int>& area)
	{
		const Point<int> lineStartPoint{area.crossYPoint() + Vector<int>{0, SectionPadding.y}};
		const Point<int> lineEndPoint{area.endPoint() + Vector<int>{0, SectionPadding.y}};
		Utility<Renderer>::get().drawLine(lineStartPoint, lineEndPoint, constants::PrimaryTextColor);
	}

	Rectangle<int> getCategorySlice(const int imageWidth, const int iconIndex)
	{
		const int columns = imageWidth / CategoryIconSize.x;
		const Point<int> sliceStartPosition
		{
			(iconIndex % columns) * CategoryIconSize.x,
			(iconIndex / columns) * CategoryIconSize.y
		};

		return {sliceStartPosition, CategoryIconSize};
	}

	Point<int> getIconTextureCoords(const Technology& tech, const int textureWidth)
	{
		const auto columns = textureWidth / TopicIconSize.x;

		return {
			(tech.iconIndex % columns) * TopicIconSize.x,
			(tech.iconIndex / columns) * TopicIconSize.y};
	}
}


ResearchReport::ResearchReport(TakeMeThereDelegate takeMeThereHandler) :
	mTakeMeThereHandler{takeMeThereHandler},
	fontMedium{fontCache.load(constants::FontPrimary, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryHuge)},
	imageLab{imageCache.load("ui/interface/lab_ug.png")},
	imageUiIcons{imageCache.load("ui/icons.png")},
	imageCategoryIcons{imageCache.load("categoryicons.png")},
	imageTopicIcons{imageCache.load("topicicons.png")},
	lstResearchTopics{{this, &ResearchReport::handleTopicChanged}},
	txtTopicDescription{fontCache.load(constants::FontPrimary, constants::FontPrimaryMedium), constants::PrimaryTextColor}
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().connect({this, &ResearchReport::onMouseDown});

	add(lstResearchTopics, {});

	add(txtTopicDescription, {});
}


ResearchReport::~ResearchReport()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().disconnect({this, &ResearchReport::onMouseDown});
}


bool ResearchReport::canView(const Structure& structure)
{
	return dynamic_cast<const ResearchFacility*>(&structure);
}


void ResearchReport::selectStructure(Structure&)
{
}


void ResearchReport::clearSelected()
{
	resetCategorySelection();
	resetResearchDetails();
	fillResearchTopicsList();
}


void ResearchReport::fillLists()
{
	resetCategorySelection();
	resetResearchDetails();
	fillResearchTopicsList();
}


void ResearchReport::refresh()
{
	if (mCategoryPanels.empty()) { return; }

	checkForLabAvailability();

	adjustCategoryIconSpacing();

	resetCategorySelection();
	resetResearchDetails();
	fillResearchTopicsList();

	setSectionRects();
	setIconPositions();

	mCategoryHeaderTextPosition = {area().position + Vector<int>{SectionPadding.x * 3 + CategoryIconSize.x, SectionPadding.y}};

	lstResearchTopics.area(mResearchTopicArea);

	txtTopicDescription.text("");
	txtTopicDescription.area(mTopicDetailsArea);
}


void ResearchReport::update()
{
	auto& renderer = Utility<Renderer>::get();
	draw(renderer);
	ControlContainer::update();
}


void ResearchReport::injectTechReferences(TechnologyCatalog& catalog, ResearchTracker& tracker)
{
	mTechCatalog = &catalog;
	mResearchTracker = &tracker;

	processCategories();
}


void ResearchReport::onResize()
{
	refresh();
}


void ResearchReport::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!visible() ||
		!area().contains(position) ||
		button != NAS2D::MouseButton::Left)
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
			fillResearchTopicsList();
		}
	}

	if (!panelClickedOn && lastPanel != nullptr)
	{
		mSelectedCategory = lastPanel;
		mSelectedCategory->selected = true;
	}
}


void ResearchReport::setIconPositions()
{
	const auto startPoint{mTopicDetailsHeaderArea.startPoint()};

	mHotLabIconPosition = {startPoint + Vector<int>{0, fontBigBold.height() + SectionPadding.y}};
	mStdLabIconPosition = {startPoint + Vector<int>{(area().size.x - startPoint.x) / 2, fontBigBold.height() + SectionPadding.y}};
	mStdLabTextPosition = {mStdLabIconPosition + Vector<int>{LabTypeIconSize.x + SectionPadding.x, LabTypeIconSize.y / 2 - fontMedium.height() / 2}};
	mHotLabTextPosition = {mHotLabIconPosition + Vector<int>{LabTypeIconSize.x + SectionPadding.x, LabTypeIconSize.y / 2 - fontMedium.height() / 2}};
}


void ResearchReport::setSectionRects()
{
	mCategoryIconArea =
	{
		mCategoryPanels.begin()->rect.startPoint(),
		{
			CategoryIconSize.x,
			mCategoryPanels.rbegin()->rect.endPoint().y - mCategoryPanels.begin()->rect.startPoint().y
		}
	};

	mResearchTopicArea =
	{
		{
			area().position.x + MarginSize * 3 + CategoryIconSize.x,
			area().position.y + fontBigBold.height() + MarginSize * 3
		},

		{
			((area().size.x / 3) * 2) - (MarginSize * 4) - CategoryIconSize.x,
			area().size.y - MarginSize * 4 - fontBigBold.height()
		}
	};

	
	mTopicDetailsHeaderArea =
	{
		area().position + Vector<int>{SectionPadding.x * 2 + mResearchTopicArea.endPoint().x, SectionPadding.y},
		{
			area().size.x - mResearchTopicArea.endPoint().x - SectionPadding.x * 3,
			100
		}
	};

	mTopicDetailsIconPosition =
	{
		mTopicDetailsHeaderArea.position.x,
		mTopicDetailsHeaderArea.crossYPoint().y + SectionPadding.y * 2
	};

	const Point<int> topicDetailStart{mTopicDetailsIconPosition + Vector<int>{0, TopicIconSize.y + SectionPadding.y}};
	mTopicDetailsArea =
	{
		topicDetailStart,
		{
			mTopicDetailsHeaderArea.size.x,
			area().size.y - topicDetailStart.y + SectionPadding.x * 4
		}
	};
}


void ResearchReport::adjustCategoryIconSpacing()
{
	const int minimumHeight = CategoryIconSize.x * (static_cast<int>(mCategoryPanels.size()));
	const int padding = ((area().size.y - 20) - minimumHeight) / static_cast<int>(mCategoryPanels.size() - 1);

	for (size_t i = 0; i < mCategoryPanels.size(); ++i)
	{
		const NAS2D::Point<int> point
		{
			area().position.x + MarginSize,
			area().position.y + MarginSize + static_cast<int>(i) * CategoryIconSize.y + static_cast<int>(i) * padding
		};
		
		mCategoryPanels[i].rect = {point, CategoryIconSize};
	}
}


void ResearchReport::checkForLabAvailability()
{
	auto& mgr = Utility<StructureManager>::get();
	mLabsAvailable = {mgr.countInState<Laboratory>(StructureState::Operational), mgr.countInState<HotLaboratory>(StructureState::Operational)};
}


void ResearchReport::processCategories()
{
	for (const auto& category : mTechCatalog->categories())
	{
		const Rectangle<int> sliceRect{getCategorySlice(imageCategoryIcons.size().x, category.icon_index)};
		mCategoryPanels.emplace_back(CategoryPanel{{}, sliceRect, category.name, false});
	}

	std::sort(mCategoryPanels.begin(), mCategoryPanels.end());
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
}


void ResearchReport::handleTopicChanged()
{
	txtTopicDescription.text("");

	if (!lstResearchTopics.isItemSelected())
	{
		return;
	}

	const auto& technology = mTechCatalog->technologyFromId(lstResearchTopics.selected().userData);
	txtTopicDescription.text(technology.description);
	mTopicDetailsIconUV = getIconTextureCoords(technology, imageTopicIcons.size().x);
}


void ResearchReport::drawCategories(NAS2D::Renderer& renderer) const
{
	for (const auto& panel : mCategoryPanels)
	{
		const auto panelRect = Rectangle<int>::Create(
			panel.rect.position - CategorySelectorPadding,
			panel.rect.endPoint() + CategorySelectorPadding);

		if (panel.selected)
		{
			renderer.drawBoxFilled(panelRect, constants::PrimaryColorVariant);
		}
		else if (panel.rect.contains(MOUSE_COORDS))
		{
			renderer.drawBoxFilled(panelRect, constants::HighlightColor);
		}

		renderer.drawSubImage(imageCategoryIcons, panel.rect.position, panel.imageSlice);
	}
}


void ResearchReport::drawCategoryHeader(NAS2D::Renderer& renderer) const
{
	renderer.drawText(fontBigBold, mSelectedCategory->name, mCategoryHeaderTextPosition, constants::PrimaryTextColor);
}


void ResearchReport::drawVerticalSectionSpacer(NAS2D::Renderer& renderer, const int startX) const
{
	const Point<int> start{startX, area().position.y + SectionPadding.y};
	const Point<int> end{startX, area().position.y + area().size.y - SectionPadding.y};
	renderer.drawLine(start, end, constants::PrimaryColor);
}


void ResearchReport::drawTopicLabRequirements(NAS2D::Renderer& renderer) const
{
	renderer.drawSubImage(imageUiIcons, mStdLabIconPosition, StandardLabIconRect);
	renderer.drawSubImage(imageUiIcons, mHotLabIconPosition, HotLabIconRect);
	renderer.drawText(fontMedium, "0 of X", mStdLabTextPosition, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, "0 of X", mHotLabTextPosition, constants::PrimaryTextColor);
}


void ResearchReport::drawTopicHeaderPanel(NAS2D::Renderer& renderer) const
{
	if (!lstResearchTopics.isItemSelected()) { return; }

	renderer.drawText(fontBigBold, constants::ResearchReportTopicDetails, mTopicDetailsHeaderArea.startPoint(), constants::PrimaryTextColor);

	drawTopicLabRequirements(renderer);
	drawDetailsHeaderSeparator(mTopicDetailsHeaderArea);
}


void ResearchReport::drawTopicDetailsPanel(NAS2D::Renderer& renderer) const
{
	if (!lstResearchTopics.isItemSelected()) { return; }

	renderer.drawSubImage(imageTopicIcons, mTopicDetailsIconPosition, {mTopicDetailsIconUV, TopicIconSize});
}


void ResearchReport::draw(NAS2D::Renderer& renderer) const
{
	drawCategories(renderer);
	drawVerticalSectionSpacer(renderer, mCategoryPanels.front().rect.endPoint().x + SectionPadding.x);
	drawCategoryHeader(renderer);
	drawVerticalSectionSpacer(renderer, (area().size.x / 3) * 2);
	drawTopicHeaderPanel(renderer);
	drawTopicDetailsPanel(renderer);
}
