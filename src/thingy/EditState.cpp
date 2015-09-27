#include "EditState.h"


EditState::EditState():	mFont("fonts/Fresca-Regular.ttf"),
						mMousePointer("ui/normal.png"),
						mThingCatalog("things/things.xml")
{}


EditState::~EditState()
{}


void EditState::initialize()
{

	// Catagories List Layout
	mMenuCatagories.font(mFont);
	mMenuCatagories.position(10, 10);
	mMenuCatagories.width(150);

	StringList catagoryList = mThingCatalog.catagories();
	for(size_t i = 0; i < catagoryList.size(); i++)
		mMenuCatagories.addItem(catagoryList[i]);

	mTxtCatagoryName.font(mFont);
	mTxtCatagoryName.position(mMenuCatagories.rect().x, mMenuCatagories.rect().y + mMenuCatagories.rect().h);
	mTxtCatagoryName.width(mMenuCatagories.rect().w);
	mTxtCatagoryName.text(mMenuCatagories.selectionText());
	mTxtCatagoryName.border(TextField::ALWAYS);

	mBtnCatagoryAdd.font(mFont);
	mBtnCatagoryAdd.position(mTxtCatagoryName.rect().x, mTxtCatagoryName.rect().y + mTxtCatagoryName.rect().h);
	mBtnCatagoryAdd.size(mTxtCatagoryName.rect().w / 2, mFont.height() + 6);
	mBtnCatagoryAdd.text("Add");

	mBtnCatagoryDelete.font(mFont);
	mBtnCatagoryDelete.position(mBtnCatagoryAdd.rect().x + mBtnCatagoryAdd.rect().w, mTxtCatagoryName.rect().y + mTxtCatagoryName.rect().h);
	mBtnCatagoryDelete.size(mTxtCatagoryName.rect().w / 2, mFont.height() + 6);
	mBtnCatagoryDelete.text("Delete");


	// Things List Layout

	mMenuThings.font(mFont);
	mMenuThings.position(10, mBtnCatagoryAdd.rect().y + mBtnCatagoryAdd.rect().h + 5);
	mMenuThings.width(150);

	StringList things = mThingCatalog.things_in_catagory(catagoryList[mMenuCatagories.currentSelection()]);

	for(size_t i = 0; i < things.size(); i++)
		mMenuThings.addItem(things[i]);

	// Event Hooks
	Utility<EventHandler>::get().mouseMotion().connect(this, &EditState::onMouseMove);
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &EditState::onMouseDown);
	Utility<EventHandler>::get().keyDown().connect(this, &EditState::onKeyDown);

	Utility<EventHandler>::get().mouseMotion().connect(&mMenuCatagories, &Menu::onMouseMove);
	Utility<EventHandler>::get().mouseMotion().connect(&mMenuThings, &Menu::onMouseMove);

	Utility<EventHandler>::get().mouseButtonDown().connect(&mMenuCatagories, &Menu::onMouseDown);
	Utility<EventHandler>::get().mouseButtonDown().connect(&mMenuThings, &Menu::onMouseDown);

	mMenuCatagories.selectionChanged().connect(this, &EditState::catagoryMenuChanged);
	mMenuThings.selectionChanged().connect(this, &EditState::thingMenuChanged);

	//list = mThingCatalog.things_in_catagory("Plants");
}


State* EditState::update()
{
	Renderer& r = Utility<Renderer>::get();
	r.clearScreen(COLOR_BLACK);

	mMenuCatagories.update();
	mMenuThings.update();
	mTxtCatagoryName.update();
	mBtnCatagoryAdd.update();
	mBtnCatagoryDelete.update();

	r.drawImage(mMousePointer, mMousePosition.x, mMousePosition.y);

	return this;
}


void EditState::onMouseMove(int x, int y, int relX, int relY)
{
	mMousePosition(x, y);
}


void EditState::onMouseDown(MouseButton button, int x, int y)
{
}


void EditState::onKeyDown(KeyCode key, KeyModifier mod)
{
	if(key == KEY_ESCAPE)
		postQuitEvent();
}


void EditState::catagoryMenuChanged()
{
	mMenuThings.dropAllItems();

	StringList things = mThingCatalog.things_in_catagory(mThingCatalog.catagories()[mMenuCatagories.currentSelection()]);

	for(size_t i = 0; i < things.size(); i++)
		mMenuThings.addItem(things[i]);
}


void EditState::thingMenuChanged()
{
}
