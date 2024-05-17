#pragma once

#include "ReportInterface.h"

#include "../../MapObjects/Structures/ResearchFacility.h"

#include <libOPHD/Technology/TechnologyCatalog.h>
#include <libOPHD/Technology/ResearchTracker.h>

#include <libControls/Button.h>
#include <libControls/ListBox.h>
#include <libControls/TextArea.h>

#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Rectangle.h>

#include <vector>


namespace NAS2D
{
	class Font;
	class Image;
}

class Warehouse;
class Structure;


class ResearchReport : public ReportInterface
{
public:
	ResearchReport();
	~ResearchReport() override;

	void fillLists() override;
	void clearSelected() override;

	void refresh() override;

	void selectStructure(Structure*) override;

	void injectTechReferences(TechnologyCatalog&, ResearchTracker&);

	void update() override;

private:
	void onResize() override;

	void onMouseDown(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position) override;
	void handleMouseDownInCategories(NAS2D::Point<int>& position);

	void setSectionRects();
	void adjustCategoryIconSpacing();

	void resetCategorySelection();

	void handleCategoryChanged();
	void handleTopicChanged();

	void drawCategories() const;
	void drawCategoryHeader() const;
	void drawVerticalSectionSpacer(const int column) const;
	void drawTopicHeaderPanel() const;
	void draw() const override;

private:
	struct CategoryPanel
	{
		NAS2D::Rectangle<int> rect{};
		NAS2D::Rectangle<int> imageSlice{};
		std::string name{};
		bool selected{false};
	};

private:
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

	NAS2D::Rectangle<int> mCategoryIconArea{};
	NAS2D::Rectangle<int> mResearchTopicArea{};
	NAS2D::Rectangle<int> mTopicDetailsHeaderArea{};
	NAS2D::Rectangle<int> mTopicDetailsArea{};
};
