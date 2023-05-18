#include "ResearchReport.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>

#include "../../Cache.h"


ResearchReport::ResearchReport():
	fontMedium{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryHuge)},
	imageLab{imageCache.load("ui/interface/lab_ug.png")},
	imageUiIcons{imageCache.load("ui/icons.png")},
	imageCategoryIcons{imageCache.load("categoryicons.png")},
	imageTopicIcons{imageCache.load("topicicons.png")}
{
}


ResearchReport::~ResearchReport()
{
}


void ResearchReport::fillLists()
{
}


void ResearchReport::clearSelected()
{
}


void ResearchReport::refresh()
{
	// determine icon placements here
}


void ResearchReport::selectStructure(Structure*)
{
}


void ResearchReport::injectTechReferences(TechnologyCatalog& catalog, ResearchTracker& tracker)
{
    mTechCatalog = &catalog;
    mResearchTracker = &tracker;
}


void ResearchReport::update()
{
	drawReport();
}


void ResearchReport::drawReport()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	renderer.drawImage(imageLab, rect().startPoint() + NAS2D::Vector<int>{10, 10});

	renderer.drawImage(imageCategoryIcons, {100, 100});
	renderer.drawImage(imageTopicIcons, {200, 200});
}