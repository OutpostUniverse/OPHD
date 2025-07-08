#include "NavControl.h"

#include "../Cache.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../Map/MapView.h"

#include <libOPHD/DirectionOffset.h>

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Resource/Font.h>


extern NAS2D::Point<int> MOUSE_COORDS;

namespace
{
	std::string levelString(int currentDepth)
	{
		if (currentDepth == 0)
		{
			return "Surface";
		}
		return "Underground " + std::to_string(currentDepth);
	}
}


NavControl::NavControl(MapView& mapView) :
	mMapView{mapView},
	mUiIcons{imageCache.load("ui/icons.png")}
{
	onMove({0, 0});
	size(NAS2D::Vector{(32 + constants::MarginTight) * 3, 99});
}


void NavControl::onMove(NAS2D::Vector<int> displacement)
{
	Control::onMove(displacement);

	// NAVIGATION BUTTONS
	const auto position = mRect.position;
	const auto navIconSpacing = 32 + constants::MarginTight;
	// Top line
	mMoveWestIconRect = {{position.x, position.y + 8}, {32, 16}};
	mMoveNorthIconRect = {{position.x + navIconSpacing, position.y + 8}, {32, 16}};
	mMoveUpIconRect = {{position.x + 2 * navIconSpacing, position.y}, {32, 32}};
	// Bottom line
	mMoveSouthIconRect = {{position.x, position.y + navIconSpacing + 8}, {32, 16}};
	mMoveEastIconRect = {{position.x + navIconSpacing, position.y + navIconSpacing + 8}, {32, 16}};
	mMoveDownIconRect = {{position.x + 2 * navIconSpacing, position.y + navIconSpacing}, {32, 32}};
}


void NavControl::onClick(NAS2D::Point<int> mousePosition)
{
	const std::array directionOptions
	{
		std::tuple{mMoveNorthIconRect, MapOffsetNorth},
		std::tuple{mMoveSouthIconRect, MapOffsetSouth},
		std::tuple{mMoveEastIconRect, MapOffsetEast},
		std::tuple{mMoveWestIconRect, MapOffsetWest},
		std::tuple{mMoveUpIconRect, MapOffsetUp},
		std::tuple{mMoveDownIconRect, MapOffsetDown},
	};

	for (const auto& [iconRect, direction] : directionOptions)
	{
		if (iconRect.contains(mousePosition))
		{
			mMapView.moveView(direction);
		}
	}
}


/**
 * Draws navigation UI.
 */
void NavControl::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	const std::array buttonDrawRects
	{
		std::tuple{mMoveDownIconRect, NAS2D::Rectangle<int>{{64, 128}, {32, 32}}},
		std::tuple{mMoveUpIconRect, NAS2D::Rectangle<int>{{96, 128}, {32, 32}}},
		std::tuple{mMoveEastIconRect, NAS2D::Rectangle<int>{{32, 128}, {32, 16}}},
		std::tuple{mMoveWestIconRect, NAS2D::Rectangle<int>{{32, 144}, {32, 16}}},
		std::tuple{mMoveNorthIconRect, NAS2D::Rectangle<int>{{0, 128}, {32, 16}}},
		std::tuple{mMoveSouthIconRect, NAS2D::Rectangle<int>{{0, 144}, {32, 16}}},
	};
	for (const auto& [currentIconRect, subImageRect] : buttonDrawRects)
	{
		bool isMouseInIcon = currentIconRect.contains(MOUSE_COORDS);
		NAS2D::Color color = isMouseInIcon ? NAS2D::Color::Red : NAS2D::Color::White;
		renderer.drawSubImage(mUiIcons, currentIconRect.position, subImageRect, color);
	}

	// Display the levels "bar"
	const auto& font = Control::getDefaultFont();
	const auto stepSizeWidth = font.width("IX");
	auto position = mRect.endPoint() - NAS2D::Vector{5, 30 - constants::Margin};
	for (int i = mMapView.maxDepth(); i >= 0; i--)
	{
		const auto levelString = (i == 0) ? std::string{"S"} : std::to_string(i);
		const auto textSize = font.size(levelString);
		bool isCurrentDepth = i == mMapView.currentDepth();
		NAS2D::Color color = isCurrentDepth ? NAS2D::Color::Red : NAS2D::Color{200, 200, 200};
		renderer.drawText(font, levelString, position - textSize, color);
		position.x -= stepSizeWidth;
	}

	// Explicit current level
	const auto& currentLevelString = levelString(mMapView.currentDepth());
	const auto& fontBoldMedium = fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryMedium);
	const auto currentLevelPosition = mRect.endPoint() - fontBoldMedium.size(currentLevelString) - NAS2D::Vector{constants::Margin, constants::Margin};
	renderer.drawText(fontBoldMedium, currentLevelString, currentLevelPosition, NAS2D::Color::White);
}
