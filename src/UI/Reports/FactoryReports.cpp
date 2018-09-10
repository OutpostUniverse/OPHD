// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "FactoryReport.h"

#include "../../Constants.h"
#include "../../FontManager.h"

#include "../../Things/Structures/SurfaceFactory.h"
#include "../../Things/Structures/SeedFactory.h"
#include "../../Things/Structures/UndergroundFactory.h"

#include <array>

static int SORT_BY_PRODUCT_POSITION = 0;
static int STATUS_LABEL_POSITION = 0;
static int WIDTH_RESOURCES_REQUIRED_LABEL = 0;

static Rectangle_2d	FACTORY_LISTBOX;
static Rectangle_2d DETAIL_PANEL;

static Font* FONT = nullptr;
static Font* FONT_BOLD = nullptr;
static Font* FONT_MED = nullptr;
static Font* FONT_MED_BOLD = nullptr;
static Font* FONT_BIG = nullptr;
static Font* FONT_BIG_BOLD = nullptr;

static Factory* SELECTED_FACTORY = nullptr;

static Image* FACTORY_SEED = nullptr;
static Image* FACTORY_AG = nullptr;
static Image* FACTORY_UG = nullptr;
static Image* FACTORY_IMAGE = nullptr;
static Image* FACTORY_PRODUCT = nullptr;

std::array<Image*, PRODUCT_COUNT> PRODUCT_IMAGE_ARRAY;
static Image* _PRODUCT_NONE = nullptr;


static std::string FACTORY_STATUS;
static const std::string RESOURCES_REQUIRED = "Resources Required";

/**
 * C'tor
 */
FactoryReport::FactoryReport()
{
	init();
}


/**
 * D'tor
 */
FactoryReport::~FactoryReport()
{
	delete FACTORY_SEED;
	delete FACTORY_AG;
	delete FACTORY_UG;
	delete _PRODUCT_NONE;

	for (auto img : PRODUCT_IMAGE_ARRAY) { delete img; }
}


/**
 * Sets up UI positions.
 */
void FactoryReport::init()
{
	FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, 10);
	FONT_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, 10);

	FONT_MED = Utility<FontManager>::get().font(constants::FONT_PRIMARY, 14);
	FONT_MED_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, 14);

	FONT_BIG = Utility<FontManager>::get().font(constants::FONT_PRIMARY, 20);
	FONT_BIG_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, 20);

	FACTORY_SEED	= new Image("ui/interface/factory_seed.png");
	FACTORY_AG		= new Image("ui/interface/factory_ag.png");
	FACTORY_UG		= new Image("ui/interface/factory_ug.png");

	/// \todo Decide if this is the best place to have these images live or if it should be done at program start.
	PRODUCT_IMAGE_ARRAY.fill(nullptr);
	PRODUCT_IMAGE_ARRAY[PRODUCT_DIGGER]				= new Image("ui/interface/product_robodigger.png");
	PRODUCT_IMAGE_ARRAY[PRODUCT_DOZER]				= new Image("ui/interface/product_robodozer.png");
	PRODUCT_IMAGE_ARRAY[PRODUCT_MINER]				= new Image("ui/interface/product_robominer.png");
	PRODUCT_IMAGE_ARRAY[PRODUCT_EXPLORER]			= new Image("ui/interface/product_roboexplorer.png");
	PRODUCT_IMAGE_ARRAY[PRODUCT_TRUCK]				= new Image("ui/interface/product_truck.png");
	PRODUCT_IMAGE_ARRAY[PRODUCT_ROAD_MATERIALS]		= new Image("ui/interface/product_road_materials.png");
	PRODUCT_IMAGE_ARRAY[PRODUCT_MAINTENANCE_PARTS]	= new Image("ui/interface/product_maintenance_parts.png");
	PRODUCT_IMAGE_ARRAY[PRODUCT_CLOTHING]			= new Image("ui/interface/product_clothing.png");
	PRODUCT_IMAGE_ARRAY[PRODUCT_MEDICINE]			= new Image("ui/interface/product_medicine.png");

	_PRODUCT_NONE = new Image("ui/interface/product_none.png");

	// Controls are drawn in the order in which they were inserted -- so this is here
	// to ensure that the combobox is drawn above everything else.
	add(&lstFactoryList, 10, 63);
	lstFactoryList.font(*FONT);
	lstFactoryList.selectionChanged().connect(this, &FactoryReport::lstFactoryListSelectionChanged);


	// SAMPLE DATA
	Factory* f5 = new SeedFactory();
	f5->productType(PRODUCT_DOZER);
	f5->forced_state_change(Structure::OPERATIONAL);
	lstFactoryList.addItem(f5);

	Factory* f6 = new SurfaceFactory();
	f6->forced_state_change(Structure::OPERATIONAL);
	lstFactoryList.addItem(f6);

	Factory* f7 = new SurfaceFactory();
	f7->forced_state_change(Structure::OPERATIONAL);
	lstFactoryList.addItem(f7);

	Factory* f8 = new UndergroundFactory();
	f8->forced_state_change(Structure::IDLE);
	lstFactoryList.addItem(f8);

	Factory* f0 = new SurfaceFactory();
	f0->forced_state_change(Structure::OPERATIONAL);
	f0->productType(PRODUCT_DOZER);
	f0->productionTurnsCompleted(3);
	lstFactoryList.addItem(f0);

	Factory* f1 = new UndergroundFactory();
	f1->forced_state_change(Structure::DISABLED);
	lstFactoryList.addItem(f1);

	Factory* f2 = new UndergroundFactory();
	f2->forced_state_change(Structure::DESTROYED);
	lstFactoryList.addItem(f2);

	Factory* f3 = new SurfaceFactory();
	f3->forced_state_change(Structure::DESTROYED);
	lstFactoryList.addItem(f3);
	// END SAMPLE DATA

	lstFactoryList.currentSelection(0);


	add(&btnShowAll, 10, 10);
	btnShowAll.font(*FONT);
	btnShowAll.size(75, 20);
	btnShowAll.type(Button::BUTTON_TOGGLE);
	btnShowAll.toggle(true);
	btnShowAll.text("All");
	btnShowAll.click().connect(this, &FactoryReport::btnShowAllClicked);

	add(&btnShowSurface, 87, 10);
	btnShowSurface.font(*FONT);
	btnShowSurface.size(75, 20);
	btnShowSurface.type(Button::BUTTON_TOGGLE);
	btnShowSurface.text("Surface");
	btnShowSurface.click().connect(this, &FactoryReport::btnShowSurfaceClicked);

	add(&btnShowUnderground, 164, 10);
	btnShowUnderground.font(*FONT);
	btnShowUnderground.size(75, 20);
	btnShowUnderground.type(Button::BUTTON_TOGGLE);
	btnShowUnderground.text("Underground");
	btnShowUnderground.click().connect(this, &FactoryReport::btnShowUndergroundClicked);

	add(&btnShowActive, 10, 33);
	btnShowActive.font(*FONT);
	btnShowActive.size(75, 20);
	btnShowActive.type(Button::BUTTON_TOGGLE);
	btnShowActive.text("Active");
	btnShowActive.click().connect(this, &FactoryReport::btnShowActiveClicked);

	add(&btnShowIdle, 87, 33);
	btnShowIdle.font(*FONT);
	btnShowIdle.size(75, 20);
	btnShowIdle.type(Button::BUTTON_TOGGLE);
	btnShowIdle.text("Idle");
	btnShowIdle.click().connect(this, &FactoryReport::btnShowIdleClicked);

	add(&btnShowDisabled, 164, 33);
	btnShowDisabled.font(*FONT);
	btnShowDisabled.size(75, 20);
	btnShowDisabled.type(Button::BUTTON_TOGGLE);
	btnShowDisabled.text("Disabled");
	btnShowDisabled.click().connect(this, &FactoryReport::btnShowDisabledClicked);


	int position_x = Utility<Renderer>::get().width() - 110;
	add(&btnIdle, position_x, 35);
	btnIdle.type(Button::BUTTON_TOGGLE);
	btnIdle.font(*FONT);
	btnIdle.size(100, 20);
	btnIdle.text("Idle");
	btnIdle.click().connect(this, &FactoryReport::btnIdleClicked);

	add(&btnClearProduction, position_x, 65);
	btnClearProduction.font(*FONT);
	btnClearProduction.size(100, 20);
	btnClearProduction.text("Clear Production");
	btnClearProduction.click().connect(this, &FactoryReport::btnClearProductionClicked);

	add(&btnTakeMeThere, position_x, 95);
	btnTakeMeThere.font(*FONT);
	btnTakeMeThere.size(100, 20);
	btnTakeMeThere.text("Take Me There");
	btnTakeMeThere.click().connect(this, &FactoryReport::btnTakeMeThereClicked);


	add(&cboFilterByProduct, 250, 33);
	cboFilterByProduct.font(*FONT);
	cboFilterByProduct.size(200, 20);

	cboFilterByProduct.addItem("None");
	cboFilterByProduct.addItem("Clothing");
	cboFilterByProduct.addItem("Maintenance Supplies");
	cboFilterByProduct.addItem("Medicine");
	cboFilterByProduct.addItem("Robodigger");
	cboFilterByProduct.addItem("Robodozer");
	cboFilterByProduct.addItem("Roboexplorer");
	cboFilterByProduct.addItem("Robominer");
	cboFilterByProduct.addItem("Road Materials");
	cboFilterByProduct.addItem("Truck");

	add(&lstProducts, cboFilterByProduct.rect().x() + cboFilterByProduct.rect().width() + 20, rect().y() + 230);
	lstProducts.font(*FONT);

	txtProductDescription.font(*FONT);
	txtProductDescription.height(128);
	txtProductDescription.textColor(0, 185, 0);
	txtProductDescription.text("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

	SORT_BY_PRODUCT_POSITION = cboFilterByProduct.rect().x() + cboFilterByProduct.rect().width() - FONT->width("Filter by Product");
	WIDTH_RESOURCES_REQUIRED_LABEL = FONT_MED_BOLD->width(RESOURCES_REQUIRED);

	Control::resized().connect(this, &FactoryReport::resized);
}


/**
 * 
 */
void FactoryReport::resized(Control* c)
{
	int h = height();
	FACTORY_LISTBOX.x(positionX() + 10);
	FACTORY_LISTBOX.y(cboFilterByProduct.positionY() + cboFilterByProduct.height() + 10);
	FACTORY_LISTBOX.width(cboFilterByProduct.positionX() + cboFilterByProduct.width() - 10);
	FACTORY_LISTBOX.height(height() - 74);

	lstFactoryList.size(FACTORY_LISTBOX.width(), FACTORY_LISTBOX.height());

	DETAIL_PANEL(cboFilterByProduct.rect().x() + cboFilterByProduct.rect().width() + 20,
		rect().y() + 10,
		rect().width() - (cboFilterByProduct.rect().x() + cboFilterByProduct.rect().width()) - 30,
		rect().y() + rect().height() - 69);

	STATUS_LABEL_POSITION = DETAIL_PANEL.x() + FONT_MED_BOLD->width("Status") + 158;
	
	int position_x = c->width() - 110;
	btnIdle.position(position_x, btnIdle.positionY());
	btnClearProduction.position(position_x, btnClearProduction.positionY());
	btnTakeMeThere.position(position_x, btnTakeMeThere.positionY());

	lstProducts.size(DETAIL_PANEL.width() / 3, DETAIL_PANEL.height() - 219);

	txtProductDescription.position(lstProducts.positionX() + lstProducts.width() + 158, lstProducts.positionY());
	txtProductDescription.width(rect().width() - txtProductDescription.positionX() - 30);

	//prgProductProgress.position(lstProducts.positionX() + lstProducts.width() + 20, DETAIL_PANEL.y() + 388);
	//prgProductProgress.size(rect().width() - txtProductDescription.positionX() - 30, 30);
}


/**
 * 
 */
void FactoryReport::filterButtonClicked()
{
	btnShowAll.toggle(false);
	btnShowSurface.toggle(false);
	btnShowUnderground.toggle(false);
	btnShowActive.toggle(false);
	btnShowIdle.toggle(false);
	btnShowDisabled.toggle(false);

	cboFilterByProduct.clearSelection();
}


/**
 * 
 */
void FactoryReport::btnShowAllClicked()
{
	filterButtonClicked();
	btnShowAll.toggle(true);
}


/**
 * 
 */
void FactoryReport::btnShowSurfaceClicked()
{
	filterButtonClicked();
	btnShowSurface.toggle(true);
}


/**
 * 
 */
void FactoryReport::btnShowUndergroundClicked()
{
	filterButtonClicked();
	btnShowUnderground.toggle(true);
}


/**
 * 
 */
void FactoryReport::btnShowActiveClicked()
{
	filterButtonClicked();
	btnShowActive.toggle(true);
}


/**
 * 
 */
void FactoryReport::btnShowIdleClicked()
{
	filterButtonClicked();
	btnShowIdle.toggle(true);
}


/**
 * 
 */
void FactoryReport::btnShowDisabledClicked()
{
	filterButtonClicked();
	btnShowDisabled.toggle(true);
}


/**
 * 
 */
void FactoryReport::btnIdleClicked()
{
	SELECTED_FACTORY->forceIdle(btnIdle.toggled());
	FACTORY_STATUS = structureStateDescription(SELECTED_FACTORY->state());
}


/**
 * 
 */
void FactoryReport::btnClearProductionClicked()
{

}


/**
 * 
 */
void FactoryReport::btnTakeMeThereClicked()
{

}



/**
 * 
 */
void FactoryReport::lstFactoryListSelectionChanged(Factory* _f)
{
	SELECTED_FACTORY = _f;

	if (!SELECTED_FACTORY) { return; }

	/// \fixme Ugly
	if (SELECTED_FACTORY->name() == constants::SEED_FACTORY) { FACTORY_IMAGE = FACTORY_SEED; }
	else if (SELECTED_FACTORY->name() == constants::SURFACE_FACTORY) { FACTORY_IMAGE = FACTORY_AG; }
	else if (SELECTED_FACTORY->name() == constants::UNDERGROUND_FACTORY) { FACTORY_IMAGE = FACTORY_UG; }

	FACTORY_STATUS = structureStateDescription(_f->state());

	btnIdle.toggle(_f->state() == Structure::IDLE);
	btnIdle.enabled(_f->state() == Structure::OPERATIONAL || _f->state() == Structure::IDLE);

	btnClearProduction.enabled(_f->state() == Structure::OPERATIONAL || _f->state() == Structure::IDLE);


	lstProducts.dropAllItems();
	const Factory::ProductionTypeList& _pl = _f->productList();
	for (auto item : _pl)
	{
		lstProducts.addItem(productDescription(item));
	}
	lstProducts.clearSelection();
}



/**
 * 
 */
void FactoryReport::drawDetailPane(Renderer& r)
{
	//r.drawBox(DETAIL_PANEL, 255, 255, 0);

	Color_4ub text_color(0, 185, 0, 255);

	r.drawImage(*FACTORY_IMAGE, DETAIL_PANEL.x(), DETAIL_PANEL.y() + 25);
	r.drawText(*FONT_BIG_BOLD, SELECTED_FACTORY->name(), DETAIL_PANEL.x(), DETAIL_PANEL.y() - 8, text_color.red(), text_color.green(), text_color.blue());

	r.drawText(*FONT_MED_BOLD, "Status", DETAIL_PANEL.x() + 138, DETAIL_PANEL.y() + 25, text_color.red(), text_color.green(), text_color.blue());
	r.drawText(*FONT_MED, FACTORY_STATUS, STATUS_LABEL_POSITION, DETAIL_PANEL.y() + 25, text_color.red(), text_color.green(), text_color.blue());

	r.drawText(*FONT_MED_BOLD, RESOURCES_REQUIRED, DETAIL_PANEL.x() + 138, DETAIL_PANEL.y() + 55, text_color.red(), text_color.green(), text_color.blue());

	r.drawText(*FONT_BOLD, "Common Metals", DETAIL_PANEL.x() + 138, DETAIL_PANEL.y() + 75, text_color.red(), text_color.green(), text_color.blue());
	r.drawText(*FONT_BOLD, "Common Minerals", DETAIL_PANEL.x() + 138, DETAIL_PANEL.y() + 90, text_color.red(), text_color.green(), text_color.blue());
	r.drawText(*FONT_BOLD, "Rare Metals", DETAIL_PANEL.x() + 138, DETAIL_PANEL.y() + 105, text_color.red(), text_color.green(), text_color.blue());
	r.drawText(*FONT_BOLD, "Rare Minerals", DETAIL_PANEL.x() + 138, DETAIL_PANEL.y() + 120, text_color.red(), text_color.green(), text_color.blue());

	r.drawText(*FONT, "0", DETAIL_PANEL.x() + 138 + WIDTH_RESOURCES_REQUIRED_LABEL - FONT->width("0"), DETAIL_PANEL.y() + 75, text_color.red(), text_color.green(), text_color.blue());
	r.drawText(*FONT, "0", DETAIL_PANEL.x() + 138 + WIDTH_RESOURCES_REQUIRED_LABEL - FONT->width("0"), DETAIL_PANEL.y() + 90, text_color.red(), text_color.green(), text_color.blue());
	r.drawText(*FONT, "0", DETAIL_PANEL.x() + 138 + WIDTH_RESOURCES_REQUIRED_LABEL - FONT->width("0"), DETAIL_PANEL.y() + 105, text_color.red(), text_color.green(), text_color.blue());
	r.drawText(*FONT, "0", DETAIL_PANEL.x() + 138 + WIDTH_RESOURCES_REQUIRED_LABEL - FONT->width("0"), DETAIL_PANEL.y() + 120, text_color.red(), text_color.green(), text_color.blue());
	
	std::string population = string_format("%i/%i", SELECTED_FACTORY->populationAvailable()[0], SELECTED_FACTORY->populationRequirements()[0]);

	SELECTED_FACTORY->populationAvailable()[0] == SELECTED_FACTORY->populationRequirements()[0] ? text_color(0, 185, 0, 255) : text_color(255, 0, 0, 255);

	r.drawBoxFilled(DETAIL_PANEL.x() + 136, DETAIL_PANEL.y() + 135, WIDTH_RESOURCES_REQUIRED_LABEL + 4, 15, 150, 0, 0, 255);

	r.drawText(*FONT_BOLD, "Workers", DETAIL_PANEL.x() + 138, DETAIL_PANEL.y() + 135, text_color.red(), text_color.green(), text_color.blue());
	r.drawText(*FONT, population, DETAIL_PANEL.x() + 138 + WIDTH_RESOURCES_REQUIRED_LABEL - FONT->width(population), DETAIL_PANEL.y() + 135, text_color.red(), text_color.green(), text_color.blue());
}


/**
 * 
 */
void FactoryReport::drawProductPane(Renderer& r)
{
	r.drawText(*FONT_BIG_BOLD, "Production", DETAIL_PANEL.x(), DETAIL_PANEL.y() + 180, 0, 185, 0);

	if (SELECTED_FACTORY->productType() == PRODUCT_NONE) { return; }

	int position_x = DETAIL_PANEL.x() + lstProducts.width() + 20;
	r.drawText(*FONT_BIG_BOLD, productDescription(SELECTED_FACTORY->productType()), position_x, DETAIL_PANEL.y() + 180, 0, 185, 0);
	r.drawImage(*PRODUCT_IMAGE_ARRAY[SELECTED_FACTORY->productType()], position_x, lstProducts.positionY());
	txtProductDescription.update();

	r.drawText(*FONT_BIG_BOLD, "Progress", position_x, DETAIL_PANEL.y() + 358, 0, 185, 0);
	//prgProductProgress.update();


	r.drawBox(position_x, DETAIL_PANEL.y() + 393, rect().width() - position_x - 10, 30, 0, 185, 0);
	int bar_width = 0;

	if (SELECTED_FACTORY->productType() != PRODUCT_NONE)
	{
		float turnsComplete = SELECTED_FACTORY->productionTurnsCompleted();
		float turnsToComplete = SELECTED_FACTORY->productionTurnsToComplete();
		float percentage = turnsComplete / turnsToComplete;

		bar_width = static_cast<float>(rect().width() - position_x - 18) * percentage;
	}

	r.drawBoxFilled(position_x + 4, DETAIL_PANEL.y() + 397, bar_width, 22, 0, 100, 0);
}


/**
 * 
 */
void FactoryReport::update()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawLine(cboFilterByProduct.rect().x() + cboFilterByProduct.rect().width() + 10, rect().y() + 10, cboFilterByProduct.rect().x() + cboFilterByProduct.rect().width() + 10, rect().y() + rect().height() - 10, 0, 185, 0);
	r.drawText(*FONT, "Filter by Product", SORT_BY_PRODUCT_POSITION, rect().y() + 10, 0, 185, 0);

	if (SELECTED_FACTORY) { drawDetailPane(r); drawProductPane(r); }

	UIContainer::update();
}
