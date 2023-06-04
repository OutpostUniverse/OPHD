#pragma once

#include <NAS2D/Renderer/Color.h>

#include <chrono>
#include <limits>
#include <string>


namespace constants
{
	inline constexpr std::chrono::milliseconds FadeSpeed{300};

	inline constexpr int BottomUiHeight{162};

	inline constexpr int Margin{6};
	inline constexpr int MarginTight{2};

	inline constexpr int MainButtonSize{30};
	inline constexpr int MiniMapButtonSize{20};

	inline constexpr int ResourceIconSize{16};

	inline constexpr int ResourceBoxWidth{200};

	inline constexpr auto NoSelection{std::numeric_limits<std::size_t>::max()};

	// ROBOT SELECTION UI SHEET ID
	inline constexpr int RobodiggerSheetId{1};
	inline constexpr int RobodozerSheetId{0};
	inline constexpr int RobominerSheetId{2};

	const NAS2D::Color MineColor = NAS2D::Color::Red;
	const NAS2D::Color ActiveMineColor = NAS2D::Color::Yellow;

	// =====================================
	// = MOUSE POINTERS
	// =====================================
	const std::string MousePointerNormal = "ui/pointers/normal.png";
	const std::string MousePointerPlaceTile = "ui/pointers/place_tile.png";


	// =====================================
	// = FONT STRINGS
	// =====================================
	const std::string FONT_PRIMARY = "fonts/opensans.ttf";
	const std::string FONT_PRIMARY_BOLD = "fonts/opensans-bold.ttf";


	// =====================================
	// = FONT SIZES
	// =====================================
	inline constexpr int FontPrimaryNormal{10};
	inline constexpr int FontPrimaryMedium{14};
	inline constexpr int FontPrimaryLarge{16};
	inline constexpr int FontPrimaryHuge{20};

	// =====================================
	// = FONT COLORS
	// =====================================
	const auto WarningTextColor = NAS2D::Color::Red;
}
