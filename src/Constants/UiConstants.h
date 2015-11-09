#ifndef __OPHD_UI_CONSTANTS__
#define __OPHD_UI_CONSTANTS__

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

	const int NO_SELECTION = -1;

	const NAS2D::Color_4ub MINE_COLOR(255, 0, 0, 255);
	const NAS2D::Color_4ub ACTIVE_MINE_COLOR(255, 255, 0, 255);
}


#endif