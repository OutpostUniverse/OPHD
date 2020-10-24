#include "MineReport.h"

#include "../TextRender.h"
#include "../../Cache.h"
#include "../../Constants.h"
#include "../../StructureManager.h"
#include "../../ProductionCost.h"

#include "../../Things/Structures/SurfaceFactory.h"
#include "../../Things/Structures/SeedFactory.h"
#include "../../Things/Structures/UndergroundFactory.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <map>
#include <array>


using namespace NAS2D;


namespace {
	const NAS2D::Image& productImage(ProductType productType)
	{
		static const std::map<ProductType, const Image*> productImages{
			{ProductType::PRODUCT_DIGGER, &imageCache.load("ui/interface/product_robodigger.png")},
			{ProductType::PRODUCT_DOZER, &imageCache.load("ui/interface/product_robodozer.png")},
			{ProductType::PRODUCT_MINER, &imageCache.load("ui/interface/product_robominer.png")},
			{ProductType::PRODUCT_EXPLORER, &imageCache.load("ui/interface/product_roboexplorer.png")},
			{ProductType::PRODUCT_TRUCK, &imageCache.load("ui/interface/product_truck.png")},
			{ProductType::PRODUCT_MAINTENANCE_PARTS, &imageCache.load("ui/interface/product_maintenance_parts.png")},
			{ProductType::PRODUCT_CLOTHING, &imageCache.load("ui/interface/product_clothing.png")},
			{ProductType::PRODUCT_MEDICINE, &imageCache.load("ui/interface/product_medicine.png")}
		};
		return *productImages.at(productType);
	}
}


MineReport::MineReport() :
	font{ fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL) },
	fontMedium{ fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_MEDIUM) },
	fontMediumBold{ fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_MEDIUM) },
	fontBigBold{ fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_HUGE) },
	btnShowAll{ "All" },
	btnShowActive{ "Active" },
	btnShowIdle{ "Idle" },
	btnShowTappedOut{ "Tapped Out" },
	btnShowDisabled{ "Disabled" },
	btnApply{ "Apply" }
{

	add(&btnShowAll, 10, 10);
	btnShowAll.size({ 75, 20 });
	btnShowAll.type(Button::Type::BUTTON_TOGGLE);
	btnShowAll.toggle(true);
	btnShowAll.click().connect(this, &MineReport::btnShowAllClicked);

	add(&btnShowActive, 87, 10);
	btnShowActive.size({ 75, 20 });
	btnShowActive.type(Button::Type::BUTTON_TOGGLE);
	btnShowActive.click().connect(this, &MineReport::btnShowActiveClicked);

	add(&btnShowIdle, 164, 10);
	btnShowIdle.size({ 75, 20 });
	btnShowIdle.type(Button::Type::BUTTON_TOGGLE);
	btnShowIdle.click().connect(this, &MineReport::btnShowIdleClicked);

	add(&btnShowTappedOut, 241, 10);
	btnShowIdle.size({ 75, 20 });
	btnShowIdle.type(Button::Type::BUTTON_TOGGLE);
	btnShowIdle.click().connect(this, &MineReport::btnShowTappedOutClicked);

	add(&btnShowDisabled, 318, 10);
	btnShowDisabled.size({ 75, 20 });
	btnShowDisabled.type(Button::Type::BUTTON_TOGGLE);
	btnShowDisabled.click().connect(this, &MineReport::btnShowDisabledClicked);

	Control::resized().connect(this, &MineReport::resized);
	fillLists();
}


/**
 * Override of the interface provided by ReportInterface class.
 *
 * \note	Pointer
 */
void MineReport::selectStructure(Structure* structure)
{
}


void MineReport::clearSelection()
{
}


/**
 * Pass-through function to simulate clicking on the Show All button.
 */
void MineReport::refresh()
{
	btnShowAllClicked();
}


/**
 * Fills the factory list with all available factories.
 */
void MineReport::fillLists()
{
}


void MineReport::resized(Control* /*c*/)
{
}


/**
 * This has been overridden to handle some UI elements that need
 * special handling.
 */
void MineReport::visibilityChanged(bool visible)
{
}


void MineReport::filterButtonClicked()
{
	btnShowAll.toggle(false);
	btnShowActive.toggle(false);
	btnShowIdle.toggle(false);
	btnShowTappedOut.toggle(false);
	btnShowDisabled.toggle(false);
}


void MineReport::btnShowAllClicked()
{
	filterButtonClicked();
	btnShowAll.toggle(true);

	fillLists();
}


void MineReport::btnShowActiveClicked()
{
	filterButtonClicked();
	btnShowActive.toggle(true);
}


void MineReport::btnShowIdleClicked()
{
	filterButtonClicked();
	btnShowIdle.toggle(true);
}


void MineReport::btnShowTappedOutClicked()
{
	filterButtonClicked();
	btnShowTappedOut.toggle(true);
}


void MineReport::btnShowDisabledClicked()
{
	filterButtonClicked();
	btnShowDisabled.toggle(true);
}


void MineReport::update()
{
	if (!visible()) { return; }
	auto& renderer = Utility<Renderer>::get();

	const auto textColor = NAS2D::Color{ 0, 185, 0 };
	const auto positionX = 200;
	renderer.drawLine(NAS2D::Point{ positionX + 10, mRect.y + 10 }, NAS2D::Point{ positionX + 10, mRect.y + mRect.height - 10 }, textColor);
	renderer.drawText(font, "Filter by Product", NAS2D::Point{ positionX - font.width("Filter by Product"), mRect.y + 10 }, textColor);


	UIContainer::update();
}
