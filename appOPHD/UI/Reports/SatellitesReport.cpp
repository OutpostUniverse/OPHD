#include "SatellitesReport.h"

#include "../../Constants/UiConstants.h"
#include "../../Cache.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


SatellitesReport::SatellitesReport(TakeMeThereDelegate takeMeThereHandler) :
	mTakeMeThereHandler{takeMeThereHandler},
	fontMedium{fontCache.load(constants::FontPrimary, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryHuge)},
	imageNotImplemented{imageCache.load("ui/interface/ni.png")}
{
}


SatellitesReport::~SatellitesReport()
{
}


void SatellitesReport::fillLists()
{
}


void SatellitesReport::clearSelected()
{
}


void SatellitesReport::refresh()
{
}


void SatellitesReport::selectStructure(Structure*)
{
}


void SatellitesReport::update()
{
	draw();
	ControlContainer::update();
}


void SatellitesReport::onResize()
{
}


void SatellitesReport::onMouseDown(NAS2D::MouseButton /*button*/, NAS2D::Point<int> /*position*/)
{
}


void SatellitesReport::draw() const
{
	auto& renderer = Utility<Renderer>::get();

	renderer.drawImage(imageNotImplemented, area().startPoint() + Vector<int>{10, 10});
	renderer.drawText(fontBigBold, "Satellites Report", area().startPoint() + Vector<int>{148, 10}, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, "This panel intentionally left blank.", area().startPoint() + Vector<int>{148, 20 + fontBigBold.height()}, constants::PrimaryTextColor);
}
