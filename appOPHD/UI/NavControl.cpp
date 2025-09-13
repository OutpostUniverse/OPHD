#include "NavControl.h"

#include "../Cache.h"
#include "../CacheFont.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../Map/MapView.h"

#include <libOPHD/DirectionOffset.h>

#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Resource/Font.h>


extern NAS2D::Point<int> MOUSE_COORDS;

namespace
{
	constexpr auto iconSizeSmall = NAS2D::Vector{32, 16};
	constexpr auto iconSizeLarge = NAS2D::Vector{32, 32};


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
	mUiIcons{imageCache.load("ui/icons.png")},
	mFont{Control::getDefaultFont()},
	mFontMediumBold{getFontMediumBold()}
{
	onMove({0, 0});
	size(NAS2D::Vector{(iconSizeSmall.x + constants::MarginTight) * 3, iconSizeLarge.y * 2 + mFont.height() + mFontMediumBold.height()});
}


void NavControl::onMove(NAS2D::Vector<int> displacement)
{
	Control::onMove(displacement);

	// NAVIGATION BUTTONS
	const auto position = mRect.position;
	const auto navIconSpacing = iconSizeSmall.x + constants::MarginTight;
	// Top line
	mMoveWestIconRect = {{position.x, position.y + 8}, iconSizeSmall};
	mMoveNorthIconRect = {{position.x + navIconSpacing, position.y + 8}, iconSizeSmall};
	mMoveUpIconRect = {{position.x + navIconSpacing * 2, position.y}, iconSizeLarge};
	// Bottom line
	mMoveSouthIconRect = {{position.x, position.y + navIconSpacing + 8}, iconSizeSmall};
	mMoveEastIconRect = {{position.x + navIconSpacing, position.y + navIconSpacing + 8}, iconSizeSmall};
	mMoveDownIconRect = {{position.x + navIconSpacing * 2, position.y + navIconSpacing}, iconSizeLarge};
}


void NavControl::onClick(NAS2D::Point<int> mousePosition)
{
	const auto directionOptions =
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
void NavControl::draw(NAS2D::Renderer& renderer) const
{
	const auto buttonDrawRects =
	{
		std::tuple{mMoveDownIconRect, NAS2D::Rectangle{{64, 128}, iconSizeLarge}},
		std::tuple{mMoveUpIconRect, NAS2D::Rectangle{{96, 128}, iconSizeLarge}},
		std::tuple{mMoveEastIconRect, NAS2D::Rectangle{{32, 128}, iconSizeSmall}},
		std::tuple{mMoveWestIconRect, NAS2D::Rectangle{{32, 144}, iconSizeSmall}},
		std::tuple{mMoveNorthIconRect, NAS2D::Rectangle{{0, 128}, iconSizeSmall}},
		std::tuple{mMoveSouthIconRect, NAS2D::Rectangle{{0, 144}, iconSizeSmall}},
	};
	for (const auto& [currentIconRect, subImageRect] : buttonDrawRects)
	{
		bool isMouseInIcon = currentIconRect.contains(MOUSE_COORDS);
		NAS2D::Color color = isMouseInIcon ? NAS2D::Color::Red : NAS2D::Color::White;
		renderer.drawSubImage(mUiIcons, currentIconRect.position, subImageRect, color);
	}

	// Display the levels "bar"
	const auto stepSizeWidth = mFont.width("IX");
	auto position = mRect.endPoint() - NAS2D::Vector{5, 30 - constants::Margin};
	for (int i = mMapView.maxDepth(); i >= 0; i--)
	{
		const auto levelString = (i == 0) ? std::string{"S"} : std::to_string(i);
		const auto textSize = mFont.size(levelString);
		bool isCurrentDepth = i == mMapView.currentDepth();
		NAS2D::Color color = isCurrentDepth ? NAS2D::Color::Red : NAS2D::Color{200, 200, 200};
		renderer.drawText(mFont, levelString, position - textSize, color);
		position.x -= stepSizeWidth;
	}

	// Explicit current level
	const auto& currentLevelString = levelString(mMapView.currentDepth());
	const auto currentLevelPosition = mRect.endPoint() - mFontMediumBold.size(currentLevelString) - NAS2D::Vector{constants::Margin, constants::Margin};
	renderer.drawText(mFontMediumBold, currentLevelString, currentLevelPosition, NAS2D::Color::White);
}
