#pragma once

#include "Report.h"

#include <libOPHD/Technology/TechnologyCatalog.h>
#include <libOPHD/Technology/ResearchTracker.h>

#include <libControls/Button.h>
#include <libControls/ListBox.h>
#include <libControls/TextArea.h>

#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Signal/Delegate.h>

#include <vector>


namespace NAS2D
{
	class Font;
	class Image;
}

class Structure;


class ResearchReport : public Report
{
public:
	using TakeMeThereDelegate = NAS2D::Delegate<void(const Structure*)>;

	ResearchReport(TakeMeThereDelegate takeMeThereHandler);
	~ResearchReport() override;

	void selectStructure(Structure*) override {}
	void clearSelected() override;
	void fillLists() override;
	void refresh() override;

	void update() override;

	void injectTechReferences(TechnologyCatalog&, ResearchTracker&);

private:
	void onResize() override;

	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position) override;
	void handleMouseDownInCategories(NAS2D::Point<int>& position);

	void setIconPositions();
	void setSectionRects();
	void adjustCategoryIconSpacing();

	void checkForLabAvailability();

	void processCategories();
	void resetCategorySelection();

	void fillResearchTopicsList();

	void resetResearchDetails();
	void handleTopicChanged();

	void drawCategories() const;
	void drawCategoryHeader() const;
	void drawVerticalSectionSpacer(const int column) const;
	void drawTopicLabRequirements() const;
	void drawTopicHeaderPanel() const;
	void drawTopicDetailsPanel() const;
	void draw() const override;

private:
	struct CategoryPanel
	{
		NAS2D::Rectangle<int> rect{};
		NAS2D::Rectangle<int> imageSlice{};
		std::string name{};
		bool selected{false};

		bool operator<(const CategoryPanel& other) const
		{
			return name < other.name;
		}
	};

	struct LabsAvailable
	{
		unsigned int standard{};
		unsigned int hot{};
	};

private:
	TakeMeThereDelegate mTakeMeThereHandler;
	const NAS2D::Font& fontMedium;
	const NAS2D::Font& fontMediumBold;
	const NAS2D::Font& fontBigBold;

	const NAS2D::Image& imageLab;
	const NAS2D::Image& imageUiIcons;
	const NAS2D::Image& imageCategoryIcons;
	const NAS2D::Image& imageTopicIcons;

	ListBox<ListBoxItemText> lstResearchTopics;

	TextArea txtTopicDescription;

	TechnologyCatalog* mTechCatalog{nullptr};
	ResearchTracker* mResearchTracker{nullptr};

	CategoryPanel* mSelectedCategory{nullptr};
	std::vector<CategoryPanel> mCategoryPanels;

	NAS2D::Point<int> mCategoryHeaderTextPosition{};
	NAS2D::Point<int> mHotLabIconPosition{};
	NAS2D::Point<int> mHotLabTextPosition{};
	NAS2D::Point<int> mStdLabIconPosition{};
	NAS2D::Point<int> mStdLabTextPosition{};

	NAS2D::Point<int> mTopicDetailsIconUV{};
	NAS2D::Point<int> mTopicDetailsIconPosition{};

	NAS2D::Rectangle<int> mCategoryIconArea{};
	NAS2D::Rectangle<int> mResearchTopicArea{};
	NAS2D::Rectangle<int> mTopicDetailsHeaderArea{};
	NAS2D::Rectangle<int> mTopicDetailsArea{};

	LabsAvailable mLabsAvailable{};

};
