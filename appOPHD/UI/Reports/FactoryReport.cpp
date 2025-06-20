#include "FactoryReport.h"

#include "../ProgressBar.h"

#include "../TextRender.h"
#include "../../Cache.h"
#include "../../Resources.h"
#include "../../Constants/Strings.h"
#include "../../Constants/UiConstants.h"
#include "../../StructureManager.h"

#include "../../MapObjects/Structures/SurfaceFactory.h"
#include "../../MapObjects/Structures/SeedFactory.h"
#include "../../MapObjects/Structures/UndergroundFactory.h"

#include <libOPHD/ProductCatalogue.h>
#include <libOPHD/ProductionCost.h>

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <map>
#include <array>


using namespace NAS2D;


namespace
{
	constexpr auto viewFilterButtonSize = NAS2D::Vector{104, 20};
	constexpr auto viewFilterSpacing = NAS2D::Vector{viewFilterButtonSize.x + constants::MarginTight, 0};
	constexpr auto viewFilterOriginRow1 = NAS2D::Vector{10, 10};
	constexpr auto viewFilterOriginRow2 = NAS2D::Vector{10, 33};
	constexpr auto mainButtonSize = NAS2D::Vector{140, 30};

	bool productTypeInRange(ProductType productType)
	{
		return ProductType::PRODUCT_NONE < productType && productType < ProductType::PRODUCT_COUNT;
	}


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


FactoryReport::FactoryReport(TakeMeThereDelegate takeMeThereHandler) :
	ReportInterface{takeMeThereHandler},
	font{Control::getDefaultFont()},
	fontMedium{fontCache.load(constants::FontPrimary, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryHuge)},
	factorySeed{imageCache.load("ui/interface/factory_seed.png")},
	factoryAboveGround{imageCache.load("ui/interface/factory_ag.png")},
	factoryUnderGround{imageCache.load("ui/interface/factory_ug.png")},
	btnShowAll{"All", viewFilterButtonSize, {this, &FactoryReport::onShowAll}},
	btnShowSurface{"Surface", viewFilterButtonSize, {this, &FactoryReport::onShowSurface}},
	btnShowUnderground{"Underground", viewFilterButtonSize, {this, &FactoryReport::onShowUnderground}},
	btnShowActive{"Active", viewFilterButtonSize, {this, &FactoryReport::onShowActive}},
	btnShowIdle{"Idle", viewFilterButtonSize, {this, &FactoryReport::onShowIdle}},
	btnShowDisabled{"Disabled", viewFilterButtonSize, {this, &FactoryReport::onShowDisabled}},
	btnIdle{"Idle", mainButtonSize, {this, &FactoryReport::onIdle}},
	btnClearProduction{"Clear Production", mainButtonSize, {this, &FactoryReport::onClearProduction}},
	btnTakeMeThere{constants::TakeMeThere, mainButtonSize, {this, &FactoryReport::onTakeMeThere}},
	btnApply{"Apply", mainButtonSize, {this, &FactoryReport::onApply}},
	cboFilterByProduct{{this, &FactoryReport::onProductFilterSelectionChange}},
	lstFactoryList{{this, &FactoryReport::onListSelectionChange}},
	lstProducts{{this, &FactoryReport::onProductSelectionChange}},
	mTxtProductDescription{constants::PrimaryTextColor}
{
	add(lstFactoryList, {10, 63});

	add(btnShowAll, viewFilterOriginRow1);
	btnShowAll.type(Button::Type::Toggle);
	btnShowAll.toggle(true);

	add(btnShowSurface, viewFilterOriginRow1 + viewFilterSpacing);
	btnShowSurface.type(Button::Type::Toggle);

	add(btnShowUnderground, viewFilterOriginRow1 + viewFilterSpacing * 2);
	btnShowUnderground.type(Button::Type::Toggle);

	add(btnShowActive, viewFilterOriginRow2);
	btnShowActive.type(Button::Type::Toggle);

	add(btnShowIdle, viewFilterOriginRow2 + viewFilterSpacing);
	btnShowIdle.type(Button::Type::Toggle);

	add(btnShowDisabled, viewFilterOriginRow2 + viewFilterSpacing * 2);
	btnShowDisabled.type(Button::Type::Toggle);

	const auto positionX = Utility<Renderer>::get().size().x - 110;
	add(btnIdle, {positionX, 35});
	btnIdle.type(Button::Type::Toggle);

	add(btnClearProduction, {positionX, 75});

	add(btnTakeMeThere, {positionX, 115});

	add(btnApply, {0, 0});

	add(cboFilterByProduct, {330, 33});
	cboFilterByProduct.size({120, 20});

	cboFilterByProduct.addItem(constants::None, ProductType::PRODUCT_NONE);
	cboFilterByProduct.addItem(constants::Clothing, ProductType::PRODUCT_CLOTHING);
	cboFilterByProduct.addItem(constants::Maintenance, ProductType::PRODUCT_MAINTENANCE_PARTS);
	cboFilterByProduct.addItem(constants::Medicine, ProductType::PRODUCT_MEDICINE);
	cboFilterByProduct.addItem(constants::Robodigger, ProductType::PRODUCT_DIGGER);
	cboFilterByProduct.addItem(constants::Robodozer, ProductType::PRODUCT_DOZER);
	cboFilterByProduct.addItem(constants::Roboexplorer, ProductType::PRODUCT_EXPLORER);
	cboFilterByProduct.addItem(constants::Robominer, ProductType::PRODUCT_MINER);
	cboFilterByProduct.addItem(constants::Truck, ProductType::PRODUCT_TRUCK);

	add(lstProducts, {cboFilterByProduct.area().position.x + cboFilterByProduct.area().size.x + 20, mRect.position.y + 230});

	mTxtProductDescription.height(260);

	fillLists();
}


/**
 * Override of the interface provided by ReportInterface class.
 *
 * \note	Pointer
 */
void FactoryReport::selectStructure(Structure* structure)
{
	lstFactoryList.setSelected(dynamic_cast<Factory*>(structure));
}


void FactoryReport::clearSelected()
{
	lstFactoryList.clearSelected();
	selectedFactory = nullptr;
	mTxtProductDescription.text("");
}


/**
 * Pass-through function to simulate clicking on the Show All button.
 */
void FactoryReport::refresh()
{
	onShowAll();
}


/**
 * Fills the factory list with all available factories.
 */
void FactoryReport::fillLists()
{
	selectedFactory = nullptr;
	lstFactoryList.clear();
	for (auto* factory : Utility<StructureManager>::get().getStructures<Factory>())
	{
		lstFactoryList.addItem(factory);
	}
	checkFactoryActionControls();
}


/**
 * Fills the factory list based on product type.
 */
void FactoryReport::fillFactoryList(ProductType type)
{
	selectedFactory = nullptr;
	lstFactoryList.clear();
	for (auto* factory : Utility<StructureManager>::get().getStructures<Factory>())
	{
		if (factory->productType() == type)
		{
			lstFactoryList.addItem(factory);
		}
	}

	checkFactoryActionControls();
}


/**
 * Fills the factory list based on surface/subterranean
 */
void FactoryReport::fillFactoryList(bool surface)
{
	selectedFactory = nullptr;
	lstFactoryList.clear();
	for (auto* factory : Utility<StructureManager>::get().getStructures<Factory>())
	{
		if (surface && (factory->name() == constants::SurfaceFactory || factory->name() == constants::SeedFactory))
		{
			lstFactoryList.addItem(factory);
		}
		else if (!surface && factory->name() == constants::UndergroundFactory)
		{
			lstFactoryList.addItem(factory);
		}
	}

	checkFactoryActionControls();
}


/**
 * Fills the factory list based on structure state.
 */
void FactoryReport::fillFactoryList(StructureState state)
{
	selectedFactory = nullptr;
	lstFactoryList.clear();
	for (auto* factory : Utility<StructureManager>::get().getStructures<Factory>())
	{
		if (factory->state() == state)
		{
			lstFactoryList.addItem(factory);
		}
	}

	lstFactoryList.setSelection(0);
	checkFactoryActionControls();
}


/**
 * Sets visibility for factory action controls.
 */
void FactoryReport::checkFactoryActionControls()
{
	bool actionControlVisible = !lstFactoryList.isEmpty();

	btnIdle.visible(actionControlVisible);
	btnClearProduction.visible(actionControlVisible);
	btnTakeMeThere.visible(actionControlVisible);
	btnApply.visible(actionControlVisible);
	lstProducts.visible(actionControlVisible);

	if (actionControlVisible) { lstFactoryList.setSelection(0); }
}


void FactoryReport::onResize()
{
	Control::onResize();

	const auto comboEndPoint = cboFilterByProduct.area().endPoint();

	lstFactoryList.size({comboEndPoint.x - 10, mRect.size.y - 74});

	detailPanelRect = {
		{ comboEndPoint.x + 20, area().position.y + 10},
		{area().size.x - comboEndPoint.x - 30, area().position.y + mRect.size.y - 69}
	};

	int positionX = mRect.size.x - 150;
	btnIdle.position({positionX, btnIdle.position().y});
	btnClearProduction.position({positionX, btnClearProduction.position().y});
	btnTakeMeThere.position({positionX, btnTakeMeThere.position().y});

	btnApply.position({positionX, mRect.size.y + 8});

	lstProducts.size({detailPanelRect.size.x / 3, detailPanelRect.size.y - 219});

	mTxtProductDescription.position(lstProducts.area().crossXPoint() + NAS2D::Vector{158, 0});
	mTxtProductDescription.width(mRect.size.x - mTxtProductDescription.position().x - 10);
}


/**
 * This has been overridden to handle some UI elements that need
 * special handling.
 */
void FactoryReport::onVisibilityChange(bool visible)
{
	if (!selectedFactory) { return; }

	StructureState state = selectedFactory->state();
	btnApply.visible(visible && (state == StructureState::Operational || state == StructureState::Idle));
	checkFactoryActionControls();

	if (selectedProductType != ProductType::PRODUCT_NONE)
	{
		mTxtProductDescription.text(ProductCatalogue::get(selectedProductType).Description);
	}
}


void FactoryReport::filterButtonClicked(bool clearCbo)
{
	btnShowAll.toggle(false);
	btnShowSurface.toggle(false);
	btnShowUnderground.toggle(false);
	btnShowActive.toggle(false);
	btnShowIdle.toggle(false);
	btnShowDisabled.toggle(false);

	if (clearCbo) { cboFilterByProduct.clearSelected(); }
}


void FactoryReport::onShowAll()
{
	filterButtonClicked(true);
	btnShowAll.toggle(true);

	fillLists();
}


void FactoryReport::onShowSurface()
{
	filterButtonClicked(true);
	btnShowSurface.toggle(true);
	fillFactoryList(true);
}


void FactoryReport::onShowUnderground()
{
	filterButtonClicked(true);
	btnShowUnderground.toggle(true);
	fillFactoryList(false);
}


void FactoryReport::onShowActive()
{
	filterButtonClicked(true);
	btnShowActive.toggle(true);
	fillFactoryList(StructureState::Operational);
}


void FactoryReport::onShowIdle()
{
	filterButtonClicked(true);
	btnShowIdle.toggle(true);
	fillFactoryList(StructureState::Idle);
}


void FactoryReport::onShowDisabled()
{
	filterButtonClicked(true);
	btnShowDisabled.toggle(true);
	fillFactoryList(StructureState::Disabled);
}


void FactoryReport::onIdle()
{
	selectedFactory->forceIdle(btnIdle.isPressed());
}


void FactoryReport::onClearProduction()
{
	selectedFactory->productType(ProductType::PRODUCT_NONE);
	lstProducts.clearSelected();
	onProductFilterSelectionChange();
}


void FactoryReport::onTakeMeThere()
{
	mTakeMeThereSignal(selectedFactory);
}


void FactoryReport::onApply()
{
	selectedFactory->productType(selectedProductType);
	onProductFilterSelectionChange();
}


void FactoryReport::onListSelectionChange()
{
	selectedFactory = lstFactoryList.selectedFactory();

	if (!selectedFactory)
	{
		checkFactoryActionControls();
		return;
	}

	/// \fixme Ugly
	if (selectedFactory->name() == constants::SeedFactory) { factoryImage = &factorySeed; }
	else if (selectedFactory->name() == constants::SurfaceFactory) { factoryImage = &factoryAboveGround; }
	else if (selectedFactory->name() == constants::UndergroundFactory) { factoryImage = &factoryUnderGround; }

	btnIdle.toggle(selectedFactory->state() == StructureState::Idle);
	btnIdle.enabled(selectedFactory->state() == StructureState::Operational || selectedFactory->state() == StructureState::Idle);

	btnClearProduction.enabled(selectedFactory->state() == StructureState::Operational || selectedFactory->state() == StructureState::Idle);

	lstProducts.clear();
	if (selectedFactory->state() != StructureState::Destroyed)
	{
		for (auto item : selectedFactory->productList())
		{
			lstProducts.add(ProductCatalogue::get(item).Name, static_cast<int>(item));
		}
	}
	lstProducts.selectIf([productType = selectedFactory->productType()](const auto& item){ return item.userData == productType; });
	selectedProductType = selectedFactory->productType();

	mTxtProductDescription.text(productTypeInRange(selectedProductType) ? ProductCatalogue::get(selectedProductType).Description : "");

	StructureState state = selectedFactory->state();
	btnApply.visible(state == StructureState::Operational || state == StructureState::Idle);
}


void FactoryReport::onProductSelectionChange()
{
	selectedProductType = static_cast<ProductType>(lstProducts.isItemSelected() ? lstProducts.selected().userData : 0);
	mTxtProductDescription.text(ProductCatalogue::get(selectedProductType).Description);
}


void FactoryReport::onProductFilterSelectionChange()
{
	if (!cboFilterByProduct.isItemSelected()) { return; }
	filterButtonClicked(false);
	fillFactoryList(static_cast<ProductType>(cboFilterByProduct.selectionUserData()));
}


void FactoryReport::drawDetailPane(Renderer& renderer)
{
	const auto startPoint = detailPanelRect.position;
	renderer.drawImage(*factoryImage, startPoint + NAS2D::Vector{0, 25});
	renderer.drawText(fontBigBold, selectedFactory->name(), startPoint + NAS2D::Vector{0, -8}, constants::PrimaryTextColor);

	auto statusPosition = startPoint + NAS2D::Vector{138, 20};
	renderer.drawText(fontMediumBold, "Status", statusPosition, constants::PrimaryTextColor);

	bool isStatusHighlighted = selectedFactory->disabled() || selectedFactory->destroyed();
	statusPosition.x += fontMediumBold.width("Status") + 20;
	renderer.drawText(fontMedium, selectedFactory->stateDescription(), statusPosition, (isStatusHighlighted ? NAS2D::Color::Red : constants::PrimaryTextColor));

	renderer.drawText(fontMediumBold, "Resources Required", startPoint + NAS2D::Vector{138, 60}, constants::PrimaryTextColor);

	const auto labelWidth = fontMediumBold.width("Resources Required");

	// MINERAL RESOURCES
	const ProductionCost& productionCost = productCost(selectedFactory->productType());
	const std::array requiredResources{
		std::pair{ResourceNamesRefined[0], productionCost.resourceCost.resources[0]},
		std::pair{ResourceNamesRefined[1], productionCost.resourceCost.resources[1]},
		std::pair{ResourceNamesRefined[2], productionCost.resourceCost.resources[2]},
		std::pair{ResourceNamesRefined[3], productionCost.resourceCost.resources[3]},
	};
	auto position = startPoint + NAS2D::Vector{138, 80};
	for (auto [title, value] : requiredResources)
	{
		drawLabelAndValueLeftJustify(position, labelWidth, title, std::to_string(value), constants::PrimaryTextColor);
		position.y += 15;
	}

	// POPULATION
	const auto workersAvailable = selectedFactory->populationAvailable().workers;
	const auto workersRequired = selectedFactory->populationRequirements().workers;
	bool isPopulationRequirementHighlighted = workersAvailable != workersRequired;
	auto text = std::to_string(workersAvailable) + " / " + std::to_string(workersRequired);
	drawLabelAndValueLeftJustify(position, labelWidth, "Workers", text, (isPopulationRequirementHighlighted ? NAS2D::Color::Red : constants::PrimaryTextColor));
}


void FactoryReport::drawProductPane(Renderer& renderer)
{
	const auto originLeft = detailPanelRect.position + NAS2D::Vector{0, 180};
	renderer.drawText(fontBigBold, "Production", originLeft, constants::PrimaryTextColor);

	const auto originRight = originLeft + NAS2D::Vector{lstProducts.size().x + 20, 0};

	if (selectedProductType != ProductType::PRODUCT_NONE)
	{
		const auto productImagePosition = NAS2D::Point{originRight.x, lstProducts.position().y};
		renderer.drawText(fontBigBold, ProductCatalogue::get(selectedProductType).Name, originRight, constants::PrimaryTextColor);
		renderer.drawImage(productImage(selectedProductType), productImagePosition);
		mTxtProductDescription.update();
	}

	if (selectedFactory->productType() == ProductType::PRODUCT_NONE) { return; }

	const auto progressTextPosition = originRight + NAS2D::Vector{0, mRect.size.y - originRight.y - 115};
	const auto buildingProductNamePosition = progressTextPosition + NAS2D::Vector{0, 35};
	renderer.drawText(fontBigBold, "Progress", progressTextPosition, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, "Building " + ProductCatalogue::get(selectedFactory->productType()).Name, buildingProductNamePosition, constants::PrimaryTextColor);

	const auto progressBarPosition = buildingProductNamePosition + NAS2D::Vector{0, fontMedium.height()};
	const auto progressBarSize = NAS2D::Vector{mRect.size.x - originRight.x - 10, 30};
	drawProgressBar(
		selectedFactory->productionTurnsCompleted(),
		selectedFactory->productionTurnsToComplete(),
		{progressBarPosition, progressBarSize}
	);

	const auto text = std::to_string(selectedFactory->productionTurnsCompleted()) + " / " + std::to_string(selectedFactory->productionTurnsToComplete());
	const auto turnsTitlePosition = progressBarPosition + NAS2D::Vector{0, 36};
	const auto turnsTextPosition = NAS2D::Point{mRect.size.x - fontMedium.width(text) - 10, turnsTitlePosition.y};
	renderer.drawText(fontMediumBold, "Turns", turnsTitlePosition, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, text, turnsTextPosition, constants::PrimaryTextColor);
}


void FactoryReport::update()
{
	if (!visible()) { return; }
	auto& renderer = Utility<Renderer>::get();

	const auto positionX = cboFilterByProduct.area().position.x + cboFilterByProduct.area().size.x;
	renderer.drawLine(NAS2D::Point{positionX + 10, mRect.position.y + 10}, NAS2D::Point{positionX + 10, mRect.position.y + mRect.size.y - 10}, constants::PrimaryTextColor);
	renderer.drawText(font, "Filter by Product", NAS2D::Point{positionX - font.width("Filter by Product"), mRect.position.y + 10}, constants::PrimaryTextColor);

	if (selectedFactory)
	{
		drawDetailPane(renderer);
		drawProductPane(renderer);
	}

	ControlContainer::update();
}
