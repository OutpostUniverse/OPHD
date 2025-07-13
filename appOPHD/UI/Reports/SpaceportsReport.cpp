#include "SpaceportsReport.h"

#include "../../Constants/UiConstants.h"
#include "../../Cache.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

using namespace NAS2D;

SpaceportsReport::SpaceportsReport(TakeMeThereDelegate takeMeThereHandler) :
	mTakeMeThereHandler{takeMeThereHandler},
	fontMedium{fontCache.load(constants::FontPrimary, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryHuge)},
	imageNotImplemented{imageCache.load("ui/interface/ni.png")}
{
}


SpaceportsReport::~SpaceportsReport()
{
}


bool SpaceportsReport::canView(const Structure& /*structure*/)
{
	return false;
}


void SpaceportsReport::selectStructure(Structure&)
{
}


void SpaceportsReport::clearSelected()
{
}


void SpaceportsReport::fillLists()
{
}


void SpaceportsReport::refresh()
{
}


void SpaceportsReport::update()
{
	auto& renderer = Utility<Renderer>::get();
	draw(renderer);
	ControlContainer::update();
}


void SpaceportsReport::onResize()
{
}


void SpaceportsReport::onMouseDown(NAS2D::MouseButton /*button*/, NAS2D::Point<int> /*position*/)
{
}


void SpaceportsReport::draw(NAS2D::Renderer& renderer) const
{
	renderer.drawImage(imageNotImplemented, area().startPoint() + Vector<int>{10, 10});
	renderer.drawText(fontBigBold, "Spaceports Report", area().startPoint() + Vector<int>{148, 10}, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, "This panel intentionally left blank.", area().startPoint() + Vector<int>{148, 20 + fontBigBold.height()}, constants::PrimaryTextColor);
}
