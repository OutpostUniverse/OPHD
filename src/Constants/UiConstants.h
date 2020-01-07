#pragma once

#include "NAS2D/NAS2D.h"

namespace constants
{
	const int BOTTOM_UI_HEIGHT = 162;

	const int MARGIN = 6;
	const int MARGIN_TIGHT = 2;

	const int MAIN_BUTTON_SIZE = 30;
	const int MINI_MAP_BUTTON_SIZE = 20;

	const int RESOURCE_ICON_SIZE = 16;

	const int RESOURCE_BOX_WIDTH = 200;

	const size_t NO_SELECTION = SIZE_MAX;

	const int MINIMUM_DISPLAY_ITEMS = 5;

	// ROBOT SELECTION UI SHEET ID
	const int ROBODIGGER_SHEET_ID = 1;
	const int ROBODOZER_SHEET_ID = 0;
	const int ROBOMINER_SHEET_ID = 2;

	const NAS2D::Color MINE_COLOR(255, 0, 0, 255);
	const NAS2D::Color ACTIVE_MINE_COLOR(255, 255, 0, 255);

	// =====================================
	// = MOUSE POINTERS
	// =====================================
	const std::string MOUSE_POINTER_NORMAL		= "ui/pointers/normal.png";
	const std::string MOUSE_POINTER_PLACE_TILE	= "ui/pointers/place_tile.png";
	const std::string MOUSE_POINTER_INSPECT		= "ui/pointers/inspect.png";


	// =====================================
	// = FONT STRINGS
	// =====================================
	const std::string FONT_PRIMARY = "fonts/opensans.ttf";
	const std::string FONT_PRIMARY_BOLD = "fonts/opensans-bold.ttf";


	// =====================================
	// = FONT SIZES
	// =====================================
	const int FONT_PRIMARY_NORMAL = 10;
	const int FONT_PRIMARY_MEDIUM = 14;
	const int FONT_PRIMARY_LARGE = 16;
	const int FONT_PRIMARY_HUGE = 20;
}
