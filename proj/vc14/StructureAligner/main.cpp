#include <iostream>

#include <NAS2D/NAS2D.h>
#include <NAS2D/Renderer/OGL_Renderer.h>


#include "../../../src/UI/Core/ListBox.h"


using namespace NAS2D;
using namespace std;

Renderer*	R = nullptr;

// Assets/Resources
Image*		TILESET = nullptr;
Font*		TINY_FONT = nullptr;

Sprite*		STRUCTURE = nullptr;
Sprite*		TUBES = nullptr;

// GUI
ListBox*	FILE_LIST = nullptr;

// FLAGS
bool		SELECTING_FILE = true;
bool		RUNNING = false;

// Stuff
std::string	SELECTED_FILE;


int	OFFSET_X = 0;
int	OFFSET_Y = 0;

const int	TILE_WIDTH = 128;
const int	TILE_HEIGHT = 64;

const int	TILE_HALF_WIDTH = TILE_WIDTH / 2;
const int	TILE_HALF_HEIGHT = TILE_HEIGHT / 2;

const int	TILE_HEIGHT_OFFSET = 10;
const int	TILE_HEIGHT_ABSOLUTE = TILE_HEIGHT - TILE_HEIGHT_OFFSET;
const int	TILE_HEIGHT_HALF_ABSOLUTE = TILE_HEIGHT_ABSOLUTE / 2;



void fillFileList();


/**
 * Handles events posted by NAS2D.
 * 
 * Some of the updates to NAS2D's Delegate class have made it impossible to properly bind a static
 * function to an event. To overcome the limitation, this class simply responds to and forwards
 * events to the static functions in this module.
 */
class eventHandler
{
public:
	eventHandler()
	{
		Utility<EventHandler>::get().keyDown().connect(this, &eventHandler::onKeyDown);
		Utility<EventHandler>::get().quit().connect(this, &eventHandler::onQuit);
		Utility<EventHandler>::get().mouseDoubleClick().connect(this, &eventHandler::onDoubleClicked);
	}


	~eventHandler()
	{
		Utility<EventHandler>::get().keyDown().disconnect(this, &eventHandler::onKeyDown);
		Utility<EventHandler>::get().quit().disconnect(this, &eventHandler::onQuit);
	}


	void onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool repeat)
	{
		switch (key)
		{
		case EventHandler::KEY_ESCAPE:
			if (!SELECTING_FILE)
			{
				if (STRUCTURE)
				{
					delete STRUCTURE;
					STRUCTURE = nullptr;
				}

				OFFSET_X = 0;
				OFFSET_Y = 0;

				SELECTING_FILE = true;
				fillFileList();
			}
			else
			{
				postQuitEvent();
			}
			break;

		case EventHandler::KEY_LEFT:
		case EventHandler::KEY_KP4:
			--OFFSET_X;
			break;

		case EventHandler::KEY_RIGHT:
		case EventHandler::KEY_KP6:
			++OFFSET_X;
			break;

		case EventHandler::KEY_UP:
		case EventHandler::KEY_KP8:
			--OFFSET_Y;
			break;

		case EventHandler::KEY_DOWN:
		case EventHandler::KEY_2:
			++OFFSET_Y;
			break;

		default:
			break;
		}
	}


	void onDoubleClicked(EventHandler::MouseButton button, int x, int y)
	{
		if (pointInRect_f(x, y, FILE_LIST->rect()))
		{
			SELECTED_FILE = "structures/" + FILE_LIST->selectionText();
			SELECTING_FILE = false;

			if (STRUCTURE)
			{
				delete STRUCTURE;
			}
			STRUCTURE = new Sprite(SELECTED_FILE);
			STRUCTURE->play("operational");
		}
	}


	void onQuit()
	{
		RUNNING = false;
	}
};


void makeRenderer()
{
	try
	{
		Utility<Renderer>::instantiateDerived(new OGL_Renderer("Structure Align Tool"));
	}
	catch (std::exception e)
	{
		throw;
	}
	catch (...)
	{
		throw std::runtime_error("Unhandled exception occured while creating a Renderer.");
	}

	R = &Utility<Renderer>::get();
}


void fillFileList()
{
	FILE_LIST->dropAllItems();

	const StringList _list = Utility<Filesystem>::get().directoryList("structures");
	for (auto file : _list)
	{
		if (file.length() > 7 && toLowercase(file.substr(file.length() - 7)) == ".sprite" && !(toLowercase(file.substr(0, 5)) == "tubes"))
		{
			FILE_LIST->addItem(file);
		}
	}
}


int main(int argc, char *argv[])
{
	Utility<Filesystem>::get().init(argv[0], "data");
	makeRenderer();
	eventHandler ev;

	// Load Assets
	TINY_FONT = new Font("fonts/opensans.ttf", 12);
	TILESET = new Image("tsets/mars.png");

	TUBES = new Sprite("structures/tubes.sprite");
	TUBES->play("ag_intersection");

	// Set up file listbox properties.
	FILE_LIST = new ListBox();
	FILE_LIST->font(*TINY_FONT);
	FILE_LIST->size(300, 500);
	FILE_LIST->position(R->width() / 2 - 150, R->height() / 2 - 250);
	FILE_LIST->sorted(true);
	fillFileList();
	
	// main loop
	RUNNING = true;
	while (RUNNING)
	{
		Utility<EventHandler>::get().pump();
		R->clearScreen(30, 30, 30);

		if (SELECTING_FILE)
		{
			R->drawText(*TINY_FONT, "Double-click an item to load it.", 10, 10, 255, 255, 255);
			FILE_LIST->update();
		}
		else
		{
			// Draws instructions.
			R->drawText(*TINY_FONT, SELECTED_FILE, 10, 10, 255, 255, 255);
			R->drawText(*TINY_FONT, "Use arrow keys to align the structure, then copy the offsets to the sprite file.", 10, 25, 255, 255, 255);


			R->drawText(*TINY_FONT, string_format("X: %i  Y: %i", -(OFFSET_X - STRUCTURE->originX(0)), -(OFFSET_Y + STRUCTURE->originY(0))), 10, 55, 255, 255, 255);

			// Upper Row
			R->drawSubImage(*TILESET, R->center_x() - TILE_WIDTH, R->center_y() - TILE_HEIGHT_ABSOLUTE, 0, 0, TILE_WIDTH, TILE_HEIGHT);
			R->drawSubImage(*TILESET, R->center_x(), R->center_y() - TILE_HEIGHT_ABSOLUTE, 0, 0, TILE_WIDTH, TILE_HEIGHT);
			
			TUBES->update(R->center_x() - TILE_WIDTH, R->center_y() - TILE_HEIGHT_ABSOLUTE);
			TUBES->update(R->center_x(), R->center_y() - TILE_HEIGHT_ABSOLUTE);

			// Center Tile
			R->drawSubImage(*TILESET, R->center_x() - TILE_HALF_WIDTH, R->center_y() - TILE_HEIGHT_HALF_ABSOLUTE, 0, 0, TILE_WIDTH, TILE_HEIGHT);
			
			STRUCTURE->update(R->center_x() - TILE_HALF_WIDTH + OFFSET_X, R->center_y() - TILE_HEIGHT_HALF_ABSOLUTE + OFFSET_Y);
			
			//TUBES->update(R->center_x() - TILE_HALF_WIDTH, R->center_y() - TILE_HEIGHT_HALF_ABSOLUTE);

			// Lower Row
			R->drawSubImage(*TILESET, R->center_x() - TILE_WIDTH, R->center_y(), 0, 0, TILE_WIDTH, TILE_HEIGHT);
			R->drawSubImage(*TILESET, R->center_x(), R->center_y(), 0, 0, TILE_WIDTH, TILE_HEIGHT);
			
			
			TUBES->update(R->center_x() - TILE_WIDTH, R->center_y());
			TUBES->update(R->center_x(), R->center_y());

		}

		R->update();
	}


	delete TINY_FONT;
	delete TILESET;
	if (STRUCTURE) { delete STRUCTURE; }
	delete TUBES;
	delete FILE_LIST;

	return 0;
}
