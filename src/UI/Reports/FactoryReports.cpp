// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "FactoryReport.h"

#include "../../Constants.h"
#include "../../FontManager.h"


#include "../../Things/Structures/SurfaceFactory.h"
#include "../../Things/Structures/SeedFactory.h"
#include "../../Things/Structures/UndergroundFactory.h"


static int SORT_BY_PRODUCT_POSITION = 0;

static Rectangle_2d	FACTORY_LISTBOX;
static Rectangle_2d DETAIL_PANEL;

static Font* FONT = nullptr;
static Font* FONT_BIG = nullptr;
static Font* FONT_BIG_BOLD = nullptr;

static Factory* SELECTED_FACTORY = nullptr;

static Image* FACTORY_SEED = nullptr;
static Image* FACTORY_AG = nullptr;
static Image* FACTORY_UG = nullptr;
static Image* FACTORY_IMAGE = nullptr;


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
}


/**
 * Sets up UI positions.
 */
void FactoryReport::init()
{
	FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, 10);
	FONT_BIG = Utility<FontManager>::get().font(constants::FONT_PRIMARY, 20);
	FONT_BIG_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, 20);

	FACTORY_SEED = new Image("ui/interface/factory_seed.png");
	FACTORY_AG = new Image("ui/interface/factory_ag.png");
	FACTORY_UG = new Image("ui/interface/factory_ug.png");

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

	SORT_BY_PRODUCT_POSITION = cboFilterByProduct.rect().x() + cboFilterByProduct.rect().width() - FONT->width("Filter by Product");

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
void FactoryReport::lstFactoryListSelectionChanged(Factory* _f)
{
	SELECTED_FACTORY = _f;

	if (!SELECTED_FACTORY) { return; }

	/// \fixme Ugly
	if (SELECTED_FACTORY->name() == constants::SEED_FACTORY) { FACTORY_IMAGE = FACTORY_SEED; }
	else if (SELECTED_FACTORY->name() == constants::SURFACE_FACTORY) { FACTORY_IMAGE = FACTORY_AG; }
	else if (SELECTED_FACTORY->name() == constants::UNDERGROUND_FACTORY) { FACTORY_IMAGE = FACTORY_UG; }
}



/**
 * 
 */
void FactoryReport::drawDetailPane(Renderer& r)
{
	r.drawBox(DETAIL_PANEL, 255, 255, 0);
	r.drawImage(*FACTORY_IMAGE, DETAIL_PANEL.x(), DETAIL_PANEL.y());
	r.drawText(*FONT_BIG_BOLD, SELECTED_FACTORY->name(), DETAIL_PANEL.x() + 138, DETAIL_PANEL.y(), 0, 185, 0);


}


/**
 * 
 */
void FactoryReport::update()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawLine(cboFilterByProduct.rect().x() + cboFilterByProduct.rect().width() + 10, rect().y() + 10, cboFilterByProduct.rect().x() + cboFilterByProduct.rect().width() + 10, rect().y() + rect().height() - 10, 255, 255, 255);
	r.drawText(*FONT, "Filter by Product", SORT_BY_PRODUCT_POSITION, rect().y() + 10, 0, 185, 0);

	if (SELECTED_FACTORY) { drawDetailPane(r); }

	UIContainer::update();
}
