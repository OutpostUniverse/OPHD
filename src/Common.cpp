// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Common.h"
#include "Constants.h"

#include <NAS2D/NAS2D.h>

#include <iostream>

#if defined(WINDOWS) || defined(WIN32)
#include <Windows.h>
#endif


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


/**
 * Shows a message dialog box.
 */
void doNonFatalErrorMessage(const std::string& title, const std::string& msg)
{
#if defined(WINDOWS) || defined(WIN32)
	MessageBoxA(NULL, msg.c_str(), title.c_str(), MB_OK | MB_ICONERROR | MB_TASKMODAL);
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
