#pragma once

#include <NAS2D/Renderer/Color.h>

#include <limits>
#include <string>


namespace constants
{
	inline constexpr int BOTTOM_UI_HEIGHT{ 162 };

	inline constexpr int MARGIN{ 6 };
	inline constexpr int MARGIN_TIGHT{ 2 };

	inline constexpr int MAIN_BUTTON_SIZE{ 30 };
	inline constexpr int MINI_MAP_BUTTON_SIZE{ 20 };

	inline constexpr int RESOURCE_ICON_SIZE{ 16 };

	inline constexpr int RESOURCE_BOX_WIDTH{ 200 };

	inline constexpr auto NO_SELECTION{ std::numeric_limits<std::size_t>::max() };

	inline constexpr unsigned int MINIMUM_DISPLAY_ITEMS{ 5 };

	// ROBOT SELECTION UI SHEET ID
	inline constexpr int ROBODIGGER_SHEET_ID{ 1 };
	inline constexpr int ROBODOZER_SHEET_ID{ 0 };
	inline constexpr int ROBOMINER_SHEET_ID{ 2 };

	const NAS2D::Color MINE_COLOR = NAS2D::Color::Red;
	const NAS2D::Color ACTIVE_MINE_COLOR = NAS2D::Color::Yellow;

	// =====================================
	// = MOUSE POINTERS
	// =====================================
	const std::string MOUSE_POINTER_NORMAL = "ui/pointers/normal.png";
	const std::string MOUSE_POINTER_PLACE_TILE = "ui/pointers/place_tile.png";
	const std::string MOUSE_POINTER_INSPECT = "ui/pointers/inspect.png";


	// =====================================
	// = FONT STRINGS
	// =====================================
	const std::string FONT_PRIMARY = "fonts/opensans.ttf";
	const std::string FONT_PRIMARY_BOLD = "fonts/opensans-bold.ttf";


	// =====================================
	// = FONT SIZES
	// =====================================
	inline constexpr int FONT_PRIMARY_NORMAL{ 10 };
	inline constexpr int FONT_PRIMARY_MEDIUM{ 14 };
	inline constexpr int FONT_PRIMARY_LARGE{ 16 };
	inline constexpr int FONT_PRIMARY_HUGE{ 20 };

	// =====================================
	// = FONT COLORS
	// =====================================
	const auto WARNING_TEXT_COLOR = NAS2D::Color::Red;
}
