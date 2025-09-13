#include "SatellitesReport.h"

#include "../../Constants/UiConstants.h"
#include "../../CacheImage.h"
#include "../../CacheFont.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>


SatellitesReport::SatellitesReport(const StructureManager& /*structureManager*/, TakeMeThereDelegate takeMeThereHandler) :
	mTakeMeThereHandler{takeMeThereHandler},
	fontMedium{getFontMedium()},
	fontMediumBold{getFontMediumBold()},
	fontBigBold{getFontHugeBold()},
	imageNotImplemented{getImage("ui/interface/ni.png")}
{
}


SatellitesReport::~SatellitesReport()
{
}


bool SatellitesReport::canView(const Structure& /*structure*/)
{
	return false;
}


void SatellitesReport::selectStructure(Structure&)
{
}


void SatellitesReport::clearSelected()
{
}


void SatellitesReport::fillLists()
{
}


void SatellitesReport::refresh()
{
}


void SatellitesReport::update()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	draw(renderer);
	ControlContainer::update();
}


void SatellitesReport::onResize()
{
}


void SatellitesReport::draw(NAS2D::Renderer& renderer) const
{
	renderer.drawImage(imageNotImplemented, area().startPoint() + NAS2D::Vector<int>{10, 10});
	renderer.drawText(fontBigBold, "Satellites Report", area().startPoint() + NAS2D::Vector<int>{148, 10}, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, "This panel intentionally left blank.", area().startPoint() + NAS2D::Vector<int>{148, 20 + fontBigBold.height()}, constants::PrimaryTextColor);
}
