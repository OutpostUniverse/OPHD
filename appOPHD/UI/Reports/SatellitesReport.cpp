#include "SatellitesReport.h"

#include "../../Constants/UiConstants.h"
#include "../../Cache.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


SatellitesReport::SatellitesReport() :
	fontMedium{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryHuge)},
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
	UIContainer::update();
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

	renderer.drawImage(imageNotImplemented, rect().startPoint() + Vector<int>{10, 10});
	renderer.drawText(fontBigBold, "Satellites Report", rect().startPoint() + Vector<int>{148, 10}, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, "This panel intentionally left blank.", rect().startPoint() + Vector<int>{148, 20 + fontBigBold.height()}, constants::PrimaryTextColor);
}
