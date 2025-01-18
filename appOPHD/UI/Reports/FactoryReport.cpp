#include "FactoryReport.h"

#include "../TextRender.h"
#include "../../Cache.h"
#include "../../Common.h"
#include "../../Constants/Strings.h"
#include "../../Constants/UiConstants.h"
#include "../../StructureManager.h"
#include "../../ProductionCost.h"
#include "../../ProductCatalogue.h"

#include "../../MapObjects/Structures/SurfaceFactory.h"
#include "../../MapObjects/Structures/SeedFactory.h"
#include "../../MapObjects/Structures/UndergroundFactory.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <map>
#include <array>


using namespace NAS2D;


namespace
{
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


FactoryReport::FactoryReport() :
	font{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal)},
	fontMedium{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryHuge)},
	factorySeed{imageCache.load("ui/interface/factory_seed.png")},
	factoryAboveGround{imageCache.load("ui/interface/factory_ag.png")},
	factoryUnderGround{imageCache.load("ui/interface/factory_ug.png")},
	btnShowAll{"All", {this, &FactoryReport::onShowAll}},
	btnShowSurface{"Surface", {this, &FactoryReport::onShowSurface}},
	btnShowUnderground{"Underground", {this, &FactoryReport::onShowUnderground}},
	btnShowActive{"Active", {this, &FactoryReport::onShowActive}},
	btnShowIdle{"Idle", {this, &FactoryReport::onShowIdle}},
	btnShowDisabled{"Disabled", {this, &FactoryReport::onShowDisabled}},
	btnIdle{"Idle", {this, &FactoryReport::onIdle}},
	btnClearProduction{"Clear Production", {this, &FactoryReport::onClearProduction}},
	btnTakeMeThere{constants::TakeMeThere, {this, &FactoryReport::onTakeMeThere}},
	btnApply{"Apply", {this, &FactoryReport::onApply}}
{
	add(lstFactoryList, {10, 63});
	lstFactoryList.selectionChanged().connect({this, &FactoryReport::onListSelectionChange});

	add(btnShowAll, {10, 10});
	btnShowAll.size({75, 20});
	btnShowAll.type(Button::Type::Toggle);
	btnShowAll.toggle(true);

	add(btnShowSurface, {87, 10});
	btnShowSurface.size({75, 20});
	btnShowSurface.type(Button::Type::Toggle);

	add(btnShowUnderground, {164, 10});
	btnShowUnderground.size({75, 20});
	btnShowUnderground.type(Button::Type::Toggle);

	add(btnShowActive, {10, 33});
	btnShowActive.size({75, 20});
	btnShowActive.type(Button::Type::Toggle);

	add(btnShowIdle, {87, 33});
	btnShowIdle.size({75, 20});
	btnShowIdle.type(Button::Type::Toggle);

	add(btnShowDisabled, {164, 33});
	btnShowDisabled.size({75, 20});
	btnShowDisabled.type(Button::Type::Toggle);

	int position_x = Utility<Renderer>::get().size().x - 110;
	add(btnIdle, {position_x, 35});
	btnIdle.type(Button::Type::Toggle);
	btnIdle.size({140, 30});

	add(btnClearProduction, {position_x, 75});
	btnClearProduction.size({140, 30});

	add(btnTakeMeThere, {position_x, 115});
	btnTakeMeThere.size({140, 30});

	add(btnApply, {0, 0});
	btnApply.size({140, 30});

	add(cboFilterByProduct, {250, 33});
	cboFilterByProduct.size({200, 20});

	cboFilterByProduct.addItem(constants::None, ProductType::PRODUCT_NONE);
	cboFilterByProduct.addItem(constants::Clothing, ProductType::PRODUCT_CLOTHING);
	cboFilterByProduct.addItem(constants::MaintenanceSupplies, ProductType::PRODUCT_MAINTENANCE_PARTS);
	cboFilterByProduct.addItem(constants::Medicine, ProductType::PRODUCT_MEDICINE);
	cboFilterByProduct.addItem(constants::Robodigger, ProductType::PRODUCT_DIGGER);
	cboFilterByProduct.addItem(constants::Robodozer, ProductType::PRODUCT_DOZER);
	cboFilterByProduct.addItem(constants::Roboexplorer, ProductType::PRODUCT_EXPLORER);
	cboFilterByProduct.addItem(constants::Robominer, ProductType::PRODUCT_MINER);
	cboFilterByProduct.addItem(constants::Truck, ProductType::PRODUCT_TRUCK);

	cboFilterByProduct.selectionChanged().connect({this, &FactoryReport::onProductFilterSelectionChange});

	add(lstProducts, {cboFilterByProduct.rect().position.x + cboFilterByProduct.rect().size.x + 20, mRect.position.y + 230});

	mTxtProductDescription.height(128);
	mTxtProductDescription.textColor(constants::PrimaryTextColor);

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

	const auto comboEndPoint = cboFilterByProduct.rect().endPoint();

	lstFactoryList.size({comboEndPoint.x - 10, mRect.size.y - 74});

	detailPanelRect = {
		{ comboEndPoint.x + 20, rect().position.y + 10},
		{rect().size.x - comboEndPoint.x - 30, rect().position.y + mRect.size.y - 69}
	};

	int position_x = mRect.size.x - 150;
	btnIdle.position({position_x, btnIdle.positionY()});
	btnClearProduction.position({position_x, btnClearProduction.positionY()});
	btnTakeMeThere.position({position_x, btnTakeMeThere.positionY()});

	btnApply.position({position_x, mRect.size.y + 8});

	lstProducts.size({detailPanelRect.size.x / 3, detailPanelRect.size.y - 219});
	lstProducts.selectionChanged().connect({this, &FactoryReport::onProductSelectionChange});

	mTxtProductDescription.position(lstProducts.rect().crossXPoint() + NAS2D::Vector{158, 0});
	mTxtProductDescription.width(mRect.size.x - mTxtProductDescription.positionX() - 30);
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
	takeMeThereSignal()(selectedFactory);
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
	lstProducts.selectIf([productType = selectedFactory->productType()](const auto& item){ return item.tag == productType; });
	selectedProductType = selectedFactory->productType();

	if (productTypeInRange(selectedProductType))
	{
		mTxtProductDescription.text(ProductCatalogue::get(selectedProductType).Description);
	}
	else
	{
		mTxtProductDescription.text("");
	}

	StructureState state = selectedFactory->state();
	btnApply.visible(state == StructureState::Operational || state == StructureState::Idle);
}


void FactoryReport::onProductSelectionChange()
{
	selectedProductType = static_cast<ProductType>(lstProducts.isItemSelected() ? lstProducts.selected().tag : 0);
	mTxtProductDescription.text(ProductCatalogue::get(selectedProductType).Description);
}


void FactoryReport::onProductFilterSelectionChange()
{
	if (!cboFilterByProduct.isItemSelected()) { return; }
	filterButtonClicked(false);
	fillFactoryList(static_cast<ProductType>(cboFilterByProduct.selectionTag()));
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
	renderer.drawText(fontBigBold, "Production", NAS2D::Point{detailPanelRect.position.x, detailPanelRect.position.y + 180}, constants::PrimaryTextColor);

	int position_x = detailPanelRect.position.x + lstProducts.size().x + 20;

	if (selectedProductType != ProductType::PRODUCT_NONE)
	{
		renderer.drawText(fontBigBold, ProductCatalogue::get(selectedProductType).Name, NAS2D::Point{position_x, detailPanelRect.position.y + 180}, constants::PrimaryTextColor);
		renderer.drawImage(productImage(selectedProductType), NAS2D::Point{position_x, lstProducts.positionY()});
		mTxtProductDescription.update();
	}

	if (selectedFactory->productType() == ProductType::PRODUCT_NONE) { return; }

	renderer.drawText(fontBigBold, "Progress", NAS2D::Point{position_x, detailPanelRect.position.y + 358}, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, "Building " + ProductCatalogue::get(selectedFactory->productType()).Name, NAS2D::Point{position_x, detailPanelRect.position.y + 393}, constants::PrimaryTextColor);

	if (selectedFactory->productType() != ProductType::PRODUCT_NONE)
	{
		drawProgressBar(
			selectedFactory->productionTurnsCompleted(),
			selectedFactory->productionTurnsToComplete(),
			{{position_x, detailPanelRect.position.y + 413}, {mRect.size.x - position_x - 10, 30}}
		);
	}

	const auto text = std::to_string(selectedFactory->productionTurnsCompleted()) + " / " + std::to_string(selectedFactory->productionTurnsToComplete());
	renderer.drawText(fontMediumBold, "Turns", NAS2D::Point{position_x, detailPanelRect.position.y + 449}, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, text, NAS2D::Point{mRect.size.x - fontMedium.width(text) - 10, detailPanelRect.position.y + 449}, constants::PrimaryTextColor);
}


void FactoryReport::update()
{
	if (!visible()) { return; }
	auto& renderer = Utility<Renderer>::get();

	const auto positionX = cboFilterByProduct.rect().position.x + cboFilterByProduct.rect().size.x;
	renderer.drawLine(NAS2D::Point{positionX + 10, mRect.position.y + 10}, NAS2D::Point{positionX + 10, mRect.position.y + mRect.size.y - 10}, constants::PrimaryTextColor);
	renderer.drawText(font, "Filter by Product", NAS2D::Point{positionX - font.width("Filter by Product"), mRect.position.y + 10}, constants::PrimaryTextColor);

	if (selectedFactory)
	{
		drawDetailPane(renderer);
		drawProductPane(renderer);
	}

	UIContainer::update();
}
