#include "SpaceportsReport.h"

#include "../../Constants/UiConstants.h"
#include "../../CacheImage.h"
#include "../../CacheFont.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>


SpaceportsReport::SpaceportsReport(const StructureManager& /*structureManager*/, TakeMeThereDelegate takeMeThereHandler) :
	mTakeMeThereHandler{takeMeThereHandler},
	fontMedium{getFontMedium()},
	fontMediumBold{getFontMediumBold()},
	fontBigBold{getFontHugeBold()},
	imageNotImplemented{getImage("ui/interface/ni.png")}
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
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	draw(renderer);
	ControlContainer::update();
}


void SpaceportsReport::onResize()
{
}


void SpaceportsReport::draw(NAS2D::Renderer& renderer) const
{
	renderer.drawImage(imageNotImplemented, area().startPoint() + NAS2D::Vector<int>{10, 10});
	renderer.drawText(fontBigBold, "Spaceports Report", area().startPoint() + NAS2D::Vector<int>{148, 10}, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, "This panel intentionally left blank.", area().startPoint() + NAS2D::Vector<int>{148, 20 + fontBigBold.height()}, constants::PrimaryTextColor);
}
