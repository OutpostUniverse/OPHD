#include "NavControl.h"

#include "../Cache.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../Map/TileMap.h"
#include "../Map/MapView.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Resource/Font.h>


extern NAS2D::Point<int> MOUSE_COORDS;

namespace
{
	std::map<int, std::string> LevelStringTable =
	{
		{constants::DepthSurface, constants::LevelSurface},
		{constants::DepthUnderground1, constants::Levelunderground1},
		{constants::DepthUnderground2, constants::Levelunderground2},
		{constants::DepthUnderground3, constants::Levelunderground3},
		{constants::DepthUnderground4, constants::Levelunderground4}
	};
}


NavControl::NavControl(MapView& mapView, TileMap& tileMap) :
	mMapView{mapView},
	mTileMap{tileMap},
	mUiIcons{imageCache.load("ui/icons.png")}
{
	onMove({0, 0});
}


void NavControl::onMove(NAS2D::Vector<int> displacement)
{
	Control::onMove(displacement);

	// NAVIGATION BUTTONS
	const auto position = mRect.startPoint();
	const auto navIconSpacing = 32 + constants::MarginTight;
	// Top line
	mMoveWestIconRect = {position.x, position.y + 8, 32, 16};
	mMoveNorthIconRect = {position.x + navIconSpacing, position.y + 8, 32, 16};
	mMoveUpIconRect = {position.x + 2 * navIconSpacing, position.y, 32, 32};
	// Bottom line
	mMoveSouthIconRect = {position.x, position.y + navIconSpacing + 8, 32, 16};
	mMoveEastIconRect = {position.x + navIconSpacing, position.y + navIconSpacing + 8, 32, 16};
	mMoveDownIconRect = {position.x + 2 * navIconSpacing, position.y + navIconSpacing, 32, 32};
}


void NavControl::onClick(NAS2D::Point<int> mousePosition)
{
	const std::array directionOptions
	{
		std::tuple{mMoveNorthIconRect, Direction::North},
		std::tuple{mMoveSouthIconRect, Direction::South},
		std::tuple{mMoveEastIconRect, Direction::East},
		std::tuple{mMoveWestIconRect, Direction::West},
		std::tuple{mMoveUpIconRect, Direction::Up},
		std::tuple{mMoveDownIconRect, Direction::Down},
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
		std::tuple{mMoveDownIconRect, NAS2D::Rectangle{64, 128, 32, 32}},
		std::tuple{mMoveUpIconRect, NAS2D::Rectangle{96, 128, 32, 32}},
		std::tuple{mMoveEastIconRect, NAS2D::Rectangle{32, 128, 32, 16}},
		std::tuple{mMoveWestIconRect, NAS2D::Rectangle{32, 144, 32, 16}},
		std::tuple{mMoveNorthIconRect, NAS2D::Rectangle{0, 128, 32, 16}},
		std::tuple{mMoveSouthIconRect, NAS2D::Rectangle{0, 144, 32, 16}},
	};
	for (const auto& [currentIconBounds, subImageBounds] : buttonDrawRects)
	{
		bool isMouseInIcon = currentIconBounds.contains(MOUSE_COORDS);
		NAS2D::Color color = isMouseInIcon ? NAS2D::Color::Red : NAS2D::Color::White;
		renderer.drawSubImage(mUiIcons, currentIconBounds.startPoint(), subImageBounds, color);
	}

	// Display the levels "bar"
	const auto& font = fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal);
	const auto stepSizeWidth = font.width("IX");
	auto position = mRect.endPoint() - NAS2D::Vector{5, 30 - constants::Margin};
	for (int i = mTileMap.maxDepth(); i >= 0; i--)
	{
		const auto levelString = (i == 0) ? std::string{"S"} : std::to_string(i);
		const auto textSize = font.size(levelString);
		bool isCurrentDepth = i == mMapView.currentDepth();
		NAS2D::Color color = isCurrentDepth ? NAS2D::Color::Red : NAS2D::Color{200, 200, 200};
		renderer.drawText(font, levelString, position - textSize, color);
		position.x -= stepSizeWidth;
	}

	// Explicit current level
	const auto& currentLevelString = LevelStringTable[mMapView.currentDepth()];
	const auto& fontBoldMedium = fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryMedium);
	const auto currentLevelPosition = mRect.endPoint() - fontBoldMedium.size(currentLevelString) - NAS2D::Vector{constants::Margin, constants::Margin};
	renderer.drawText(fontBoldMedium, currentLevelString, currentLevelPosition, NAS2D::Color::White);
}
