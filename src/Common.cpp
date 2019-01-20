// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Common.h"
#include "Constants.h"

#include "Things/Structures/Structure.h"

#include <NAS2D/NAS2D.h>

#include <iostream>

#if defined(WINDOWS) || defined(WIN32)
#include <Windows.h>
#endif

#include "SDL.h"

using namespace NAS2D;


std::map<Structure::StructureState, Color_4ub> STRUCTURE_COLOR_TABLE
{
	{ Structure::UNDER_CONSTRUCTION,	Color_4ub(150, 150, 150, 100) },
	{ Structure::OPERATIONAL,			Color_4ub(0, 185, 0, 255) },
	{ Structure::IDLE,					Color_4ub(0, 185, 0, 100) },
	{ Structure::DISABLED,				Color_4ub(220, 0, 0, 255) },
	{ Structure::DESTROYED,				Color_4ub(220, 0, 0, 255) }
};


std::map<Structure::StructureState, Color_4ub> STRUCTURE_TEXT_COLOR_TABLE
{
	{ Structure::UNDER_CONSTRUCTION,	Color_4ub(185, 185, 185, 100) },
	{ Structure::OPERATIONAL,			Color_4ub(0, 185, 0, 255) },
	{ Structure::IDLE,					Color_4ub(0, 185, 0, 100) },
	{ Structure::DISABLED,				Color_4ub(220, 0, 0, 255) },
	{ Structure::DESTROYED,				Color_4ub(220, 0, 0, 255) }
};


std::map<int, std::string> TILE_INDEX_TRANSLATION =
{
	{ 0, constants::TILE_INDEX_TRANSLATION_BULLDOZED },
	{ 1, constants::TILE_INDEX_TRANSLATION_CLEAR },
	{ 2, constants::TILE_INDEX_TRANSLATION_ROUGH },
	{ 3, constants::TILE_INDEX_TRANSLATION_DIFFICULT },
	{ 4, constants::TILE_INDEX_TRANSLATION_IMPASSABLE },
};


std::map<MineProductionRate, std::string> MINE_YIELD_TRANSLATION =
{
	{ PRODUCTION_RATE_HIGH , constants::MINE_YIELD_HIGH },
	{ PRODUCTION_RATE_LOW , constants::MINE_YIELD_LOW },
	{ PRODUCTION_RATE_MEDIUM , constants::MINE_YIELD_MEDIUM }
};


std::map<DisabledReason, std::string> DISABLED_REASON_TABLE =
{
	{ DISABLED_NONE, "Not Disabled" },

	{ DISABLED_CHAP, "CHAP Facility unavailable" },
	{ DISABLED_DISCONNECTED, "Not connected to a Command Center" },
	{ DISABLED_ENERGY, "Insufficient Energy" },
	{ DISABLED_POPULATION, "Insufficient Population" },
	{ DISABLED_REFINED_RESOURCES, "Insufficient refned resources" },
	{ DISABLED_STRUCTURAL_INTEGRITY, "Structural Integrity Decay" }
};


/**
 * Description table for products.
 */
std::array<std::string, PRODUCT_COUNT> PRODUCT_DESCRIPTION_TABLE =
{
	constants::ROBODIGGER,
	constants::ROBODOZER,
	constants::ROBOMINER,
	constants::ROBOEXPLORER,
	constants::TRUCK,

	"PRODUCT_RESERVED_AG_05",
	"PRODUCT_RESERVED_AG_06",
	"PRODUCT_RESERVED_AG_07",

	constants::ROAD_MATERIALS,
	constants::MAINTENANCE_SUPPLIES,

	"PRODUCT_RESERVED_AG_10",
	"PRODUCT_RESERVED_AG_11",
	"PRODUCT_RESERVED_AG_12",
	"PRODUCT_RESERVED_AG_13",
	"PRODUCT_RESERVED_AG_14",
	"PRODUCT_RESERVED_AG_15",

	"PRODUCT_RESERVED_AG_16",
	"PRODUCT_RESERVED_AG_17",
	"PRODUCT_RESERVED_AG_18",
	"PRODUCT_RESERVED_AG_19",
	"PRODUCT_RESERVED_AG_20",
	"PRODUCT_RESERVED_AG_21",
	"PRODUCT_RESERVED_AG_22",
	"PRODUCT_RESERVED_AG_23",

	"PRODUCT_RESERVED_AG_24",
	"PRODUCT_RESERVED_AG_25",
	"PRODUCT_RESERVED_AG_26",
	"PRODUCT_RESERVED_AG_27",
	"PRODUCT_RESERVED_AG_28",
	"PRODUCT_RESERVED_AG_29",
	"PRODUCT_RESERVED_AG_30",
	"PRODUCT_RESERVED_AG_31",


	// =====================================
	// = UNDERGROUND FACTORIES
	// =====================================
	constants::CLOTHING,
	constants::MEDICINE,
	"PRODUCT_RESERVED_UG_34",
	"PRODUCT_RESERVED_UG_35",
	"PRODUCT_RESERVED_UG_36",
	"PRODUCT_RESERVED_UG_37",
	"PRODUCT_RESERVED_UG_38",
	"PRODUCT_RESERVED_UG_39",

	"PRODUCT_RESERVED_UG_40",
	"PRODUCT_RESERVED_UG_41",
	"PRODUCT_RESERVED_UG_42",
	"PRODUCT_RESERVED_UG_43",
	"PRODUCT_RESERVED_UG_44",
	"PRODUCT_RESERVED_UG_45",
	"PRODUCT_RESERVED_UG_46",
	"PRODUCT_RESERVED_UG_47",

	"PRODUCT_RESERVED_UG_48",
	"PRODUCT_RESERVED_UG_49",
	"PRODUCT_RESERVED_UG_50",
	"PRODUCT_RESERVED_UG_51",
	"PRODUCT_RESERVED_UG_52",
	"PRODUCT_RESERVED_UG_53",
	"PRODUCT_RESERVED_UG_54",
	"PRODUCT_RESERVED_UG_55",

	"PRODUCT_RESERVED_UG_56",
	"PRODUCT_RESERVED_UG_57",
	"PRODUCT_RESERVED_UG_58",
	"PRODUCT_RESERVED_UG_59",
	"PRODUCT_RESERVED_UG_60",
	"PRODUCT_RESERVED_UG_61",
	"PRODUCT_RESERVED_UG_62",
	"PRODUCT_RESERVED_UG_63"
};


#if defined(WINDOWS) || defined(WIN32)
#include <SDL_syswm.h>
/**
 * Gets a Windows API HWND handle to the main
 * window of the application.
 * 
 * \note	This is only exposed for Windows applications.
 * 
 * \warning	Uses internal knowledge of NAS2D to force an
 *			export that should not be exposed. This will
 *			break if NAS2D changes its structure.
 */
HWND WIN32_getWindowHandle()
{
	SDL_SysWMinfo systemInfo;
	SDL_VERSION(&systemInfo.version);

	/** \fixme Evil hack exposing an internal NAS2D variable. */
	extern SDL_Window* _WINDOW;
	if (SDL_GetWindowWMInfo(_WINDOW, &systemInfo) != 1)
	{
		return nullptr;
	}

	return systemInfo.info.win.window;
}
#endif


/**
 * Convenience function to pass a Rectangle_2df to \c isPointInRect()
 */
bool pointInRect_f(int x, int y, const Rectangle_2df& rect)
{
	return pointInRect_f(x, y, rect.x(), rect.y(), rect.width(), rect.height());
}


/**
 * Convenience function to pass \c float's to \c isPointInRect()
 */
bool pointInRect_f(int x, int y, float rectX, float rectY, float rectW, float rectH)
{
	return isPointInRect(x, y,	static_cast<int>(rectX), static_cast<int>(rectY), static_cast<int>(rectW), static_cast<int>(rectH));
}


const std::string& productDescription(ProductType type)
{
	if (type == PRODUCT_NONE) { return constants::NONE; }

	return PRODUCT_DESCRIPTION_TABLE[static_cast<size_t>(type)];
}


ProductType productTypeFromDescription(const std::string& description)
{
	for (size_t i = 0; i < PRODUCT_DESCRIPTION_TABLE.size(); ++i)
	{
		if (PRODUCT_DESCRIPTION_TABLE[i] == description)
		{
			// dubious (and slow)
			return static_cast<ProductType>(i);
		}
	}

	return PRODUCT_NONE;
}


const std::string& disabledReason(DisabledReason _d)
{
	return DISABLED_REASON_TABLE[_d];
}


Color_4ub& structureColorFromIndex(size_t index)
{
	return STRUCTURE_COLOR_TABLE[static_cast<Structure::StructureState>(index)];
}


Color_4ub& structureTextColorFromIndex(size_t index)
{
	return STRUCTURE_TEXT_COLOR_TABLE[static_cast<Structure::StructureState>(index)];
}


/**
 * Shows a message dialog box.
 */
void doNonFatalErrorMessage(const std::string& title, const std::string& msg)
{
#if defined(WINDOWS) || defined(WIN32)
	MessageBoxA(WIN32_getWindowHandle(), msg.c_str(), title.c_str(), MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), msg.c_str(), NULL);
#endif
}


/**
 * Shows a message dialog box with no icon.
 */
void doAlertMessage(const std::string& title, const std::string& msg)
{
#if defined(WINDOWS) || defined(WIN32)
	MessageBoxA(WIN32_getWindowHandle(), msg.c_str(), title.c_str(), MB_OK | /*MB_ICONINFORMATION |*/ MB_TASKMODAL);
#else
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, title.c_str(), msg.c_str(), NULL);
#endif
}


/**
 * Shows a message dialog box with Yes and No buttons.
 */
bool doYesNoMessage(const std::string& title, const std::string msg)
{
	bool yes = false;
#if defined(WINDOWS) || defined(WIN32)
	yes = (MessageBoxA(WIN32_getWindowHandle(), msg.c_str(), title.c_str(), MB_YESNO | MB_ICONINFORMATION | MB_TASKMODAL) == IDYES);
#else
	const SDL_MessageBoxButtonData buttons[] =
	{
		{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No" },
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" }
	};

	const SDL_MessageBoxData messageboxdata =
	{
		SDL_MESSAGEBOX_INFORMATION,
		nullptr,
		title.c_str(),
		msg.c_str(),
		SDL_arraysize(buttons),
		buttons,
		nullptr
	};

	int _bid = 0;
	SDL_ShowMessageBox(&messageboxdata, &_bid);
	yes = (_bid == 1);
#endif

	return yes;
}


/**
 * Checks a savegame version.
 * 
 * \throws	Throws a std::runtime_error if there are any errors with a savegame version, formation or missing root nodes.
 * 
 * \fixme	Find a more efficient way to do this.
 */
void checkSavegameVersion(const std::string& filename)
{
	File xmlFile = Utility<Filesystem>::get().open(filename);

	NAS2D::Xml::XmlDocument doc;
	doc.parse(xmlFile.raw_bytes());
	if (doc.error())
	{
		throw std::runtime_error("Malformed savegame ('" + filename + "'). Error on Row " + std::to_string(doc.errorRow()) + ", Column " + std::to_string(doc.errorCol()) + ": " + doc.errorDesc());
	}

	NAS2D::Xml::XmlElement* root = doc.firstChildElement(constants::SAVE_GAME_ROOT_NODE);
	if (root == nullptr)
	{
		throw std::runtime_error("Root element in '" + filename + "' is not '" + constants::SAVE_GAME_ROOT_NODE + "'.");
	}

	std::string sg_version = root->attribute("version");
	if (sg_version != constants::SAVE_GAME_VERSION)
	{
		throw std::runtime_error("Savegame version mismatch: '" + filename + "'. Expected " + constants::SAVE_GAME_VERSION + ", found " + sg_version + ".");
	}
}


NAS2D::StringList split_string(const char *str, char delim)
{
	std::vector<std::string> result;
	do
	{
		const char *begin = str;
		while (*str != delim && *str) { str++; }
		result.push_back(std::string(begin, str));
	} while (0 != *str++);

	return result;
}


void drawBasicProgressBar(int x, int y, int width, int height, float percent, int padding)
{
	Utility<Renderer>::get().drawBox(x, y, width, height, 0, 185, 0);

	if (percent > 0.0f)
	{
		int bar_width = static_cast<float>(width - (padding + padding)) * percent;
		Utility<Renderer>::get().drawBoxFilled(x + padding, y + padding + 1, bar_width - 1, height - (padding + padding) - 1, 0, 100, 0);
	}
}
