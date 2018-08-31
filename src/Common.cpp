// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Common.h"
#include "Constants.h"

#include <NAS2D/NAS2D.h>

#include <iostream>

#if defined(WINDOWS) || defined(WIN32)
#include <Windows.h>
#endif

#include "SDL.h"

using namespace NAS2D;


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


/**
 * Convenience function to pass a Rectangle_2df to \c isPointInRect()
 */
bool pointInRect_f(int x, int y, const Rectangle_2df& rect)
{
	return pointInRect_f(x, y, rect.x(), rect.y(), rect.width(), rect.height());
}


/**
 * Convenience function to pass a \c float's to \c isPointInRect()
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


/**
 * Shows a message dialog box.
 */
void doNonFatalErrorMessage(const std::string& title, const std::string& msg)
{
#if defined(WINDOWS) || defined(WIN32)
	MessageBoxA(NULL, msg.c_str(), title.c_str(), MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), msg.c_str(), NULL);
#endif
	
	std::cout << msg << std::endl;
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

		while (*str != delim && *str)
			str++;

		result.push_back(std::string(begin, str));
	} while (0 != *str++);

	return result;
}
