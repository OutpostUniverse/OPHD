#pragma once

#include "ReportInterface.h"

#include "../ProductListBox.h"
#include "../StructureListBox.h"

#include "../../MapObjects/Structures/ResearchFacility.h"
#include "../../Technology/TechnologyCatalog.h"
#include "../../Technology/ResearchTracker.h"

#include "../Core/Button.h"

#include <NAS2D/Math/Point.h>

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
	void onMouseDown(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position);

	void draw() const override;
	void drawCategories() const;
	void drawTopicHeader() const;
	void drawVerticalSectionSpacer(const int column) const;

	void onAllTopicsClicked();
	void onAvailableTopicsClicked();
	void onCompletedTopicsClicked();
	void onStandardLabClicked();
	void onHotLabClicked();

	const NAS2D::Font& fontMedium;
	const NAS2D::Font& fontMediumBold;
	const NAS2D::Font& fontBigBold;

	const NAS2D::Image& imageLab;
	const NAS2D::Image& imageUiIcons;
	const NAS2D::Image& imageCategoryIcons;
	const NAS2D::Image& imageTopicIcons;

	Button btnAllTopics;
	Button btnAvailableTopics;
	Button btnCompletedTopics;
	Button btnStandardLab;
	Button btnHotLab;

    
    TechnologyCatalog* mTechCatalog{ nullptr };
    ResearchTracker* mResearchTracker{ nullptr };

	Button btnTakeMeThere;

	StructureListBox lstStructures;
	ProductListBox lstProducts;

	std::size_t mLabCount{};

	ResearchFacility* mSelectedLab{ nullptr };
};
