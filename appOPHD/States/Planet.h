#pragma once

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/Timer.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Resource/Image.h>


class Planet
{
public:
	using MouseSignal = NAS2D::Signal<>;

public:
	Planet(const std::string& imagePath);
	~Planet();

	NAS2D::Point<int> position() const { return mPosition; }
	void position(const NAS2D::Point<int>& point) { mPosition = point; }

	bool mouseHovering() const { return mMouseInArea; }

	MouseSignal::Source& mouseEnter() { return mMouseEnterSignal; }
	MouseSignal::Source& mouseExit() { return mMouseExitSignal; }

	void update();

protected:
	bool pointInCircle(NAS2D::Point<int> point) const;
	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

private:
	Planet() = delete;
	Planet(const Planet&) = delete;
	Planet& operator=(const Planet&) = delete;

private:
	int mTick = 0;

	const NAS2D::Image mImage;
	NAS2D::Point<int> mPosition;

	MouseSignal mMouseEnterSignal;
	MouseSignal mMouseExitSignal;

	bool mMouseInArea = false;

	NAS2D::Timer mTimer;
};
