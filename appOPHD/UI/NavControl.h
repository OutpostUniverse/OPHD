#pragma once

#include <libControls/Control.h>

#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Math/Vector.h>


namespace NAS2D
{
	class Image;
}

class MapView;
class MapViewState;


class NavControl : public Control
{
public:
	NavControl(MapView& mapView);

	void draw() const override;

protected:
	void onMove(NAS2D::Vector<int> displacement) override;

	friend MapViewState;
	void onClick(NAS2D::Point<int> mousePosition);

private:
	MapView& mMapView;
	const NAS2D::Image& mUiIcons;

	NAS2D::Rectangle<int> mMoveNorthIconRect;
	NAS2D::Rectangle<int> mMoveSouthIconRect;
	NAS2D::Rectangle<int> mMoveEastIconRect;
	NAS2D::Rectangle<int> mMoveWestIconRect;
	NAS2D::Rectangle<int> mMoveUpIconRect;
	NAS2D::Rectangle<int> mMoveDownIconRect;
	NAS2D::Rectangle<int> mBottomUiRect;
};
