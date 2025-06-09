#pragma once

#include <NAS2D/Duration.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/Color.h>

#include <string>


namespace constants
{
	inline constexpr Duration FadeSpeed{300};

	inline constexpr Duration PlanetAnimationSpeed{35};

	inline constexpr auto MinimumWindowSize{NAS2D::Vector{1000, 700}};

	inline constexpr int BottomUiHeight{162};

	inline constexpr int Margin{6};
	inline constexpr int MarginTight{2};

	inline constexpr int MainButtonSize{30};
	inline constexpr int MiniMapButtonSize{20};

	inline constexpr int ResourceIconSize{16};

	inline constexpr int ResourceBoxWidth{200};

	inline constexpr unsigned int StructureIconSize{46};
	inline constexpr unsigned int RobotIconSize{46};

	// ROBOT SELECTION UI SHEET ID
	inline constexpr int RobodiggerSheetId{1};
	inline constexpr int RobodozerSheetId{0};
	inline constexpr int RobominerSheetId{2};

	const NAS2D::Color MineColor = NAS2D::Color::Red;
	const NAS2D::Color ActiveMineColor = NAS2D::Color::Yellow;

	const NAS2D::Color PrimaryColor{0, 185, 0};
	const NAS2D::Color PrimaryColorVariant{0, 85, 0};

	const NAS2D::Color SecondaryColor{185, 185, 0};
	const NAS2D::Color SecondaryColorVariant{100, 100, 0};

	const NAS2D::Color HighlightColor{0, 185, 185, 100};

	const NAS2D::Color PrimaryTextColor{PrimaryColor};
	const NAS2D::Color PrimaryTextColorVariant{PrimaryColorVariant};

	const NAS2D::Color WarningTextColor = NAS2D::Color::Red;

	// =====================================
	// = MOUSE POINTERS
	// =====================================
	const std::string MousePointerNormal = "ui/pointers/normal.png";
	const std::string MousePointerPlaceTile = "ui/pointers/place_tile.png";


	// =====================================
	// = FONT STRINGS
	// =====================================
	const std::string FontPrimary = "fonts/opensans.ttf";
	const std::string FontPrimaryBold = "fonts/opensans-bold.ttf";


	// =====================================
	// = FONT SIZES
	// =====================================
	inline constexpr int FontPrimaryNormal{14};
	inline constexpr int FontPrimaryMedium{18};
	inline constexpr int FontPrimaryLarge{20};
	inline constexpr int FontPrimaryHuge{24};
}
