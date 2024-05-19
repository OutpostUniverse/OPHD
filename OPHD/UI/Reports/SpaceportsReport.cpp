#include "SpaceportsReport.h"

#include "../../Constants/UiConstants.h"
#include "../../Cache.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

using namespace NAS2D;

SpaceportsReport::SpaceportsReport() :
	fontMedium{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryHuge)},
	imageNotImplemented{imageCache.load("ui/interface/ni.png")}
{
}


SpaceportsReport::~SpaceportsReport()
{
}


void SpaceportsReport::fillLists()
{
}


void SpaceportsReport::clearSelected()
{
}


void SpaceportsReport::refresh()
{
}


void SpaceportsReport::selectStructure(Structure*)
{
}


void SpaceportsReport::update()
{
	draw();
	UIContainer::update();
}


void SpaceportsReport::onResize()
{
}


void SpaceportsReport::onMouseDown(NAS2D::EventHandler::MouseButton /*button*/, NAS2D::Point<int> /*position*/)
{
}


void SpaceportsReport::draw() const
{
	auto& renderer = Utility<Renderer>::get();

	renderer.drawImage(imageNotImplemented, rect().startPoint() + Vector<int>{10, 10});
	renderer.drawText(fontBigBold, "Spaceports Report", rect().startPoint() + Vector<int>{148, 10}, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, "This panel intentionally left blank.", rect().startPoint() + Vector<int>{148, 20 + fontBigBold.height()}, constants::PrimaryTextColor);
}
