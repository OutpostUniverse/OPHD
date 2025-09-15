#pragma once

#include <NAS2D/Duration.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/Color.h>


namespace constants
{
	inline constexpr Duration FadeSpeed{300};

	inline constexpr int Margin{6};
	inline constexpr int MarginTight{2};

	inline constexpr int ResourceIconSize{16};

	const NAS2D::Color PrimaryColor{0, 185, 0};
	const NAS2D::Color PrimaryColorVariant{0, 85, 0};

	const NAS2D::Color SecondaryColor{185, 185, 0};

	const NAS2D::Color HighlightColor{0, 185, 185, 100};

	const NAS2D::Color PrimaryTextColor{PrimaryColor};

	const NAS2D::Color WarningTextColor = NAS2D::Color::Red;
}
