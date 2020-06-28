#pragma once

#include <cmath>

#include <NAS2D/Signal.h>
#include <NAS2D/Timer.h>
#include <NAS2D/Renderer/Point.h>
#include <NAS2D/Resources/Image.h>

#include "../Constants.h"




class Planet
{
public:
	enum PlanetType
	{
		PLANET_TYPE_NONE,

		PLANET_TYPE_MERCURY,
		PLANET_TYPE_MARS,
		PLANET_TYPE_GANYMEDE,

		PLANET_TYPE_COUNT
	};

public:
	using MouseCallback = NAS2D::Signals::Signal<>;

public:
	Planet(PlanetType type);
	~Planet();

	PlanetType type() const { return mType; }

	void position(const NAS2D::Point<int>& point) { mPosition = point; }
	void position(int x, int y) { mPosition = {x, y}; }

	int x() const { return mPosition.x(); }
	int y() const { return mPosition.y(); }

	int digDepth() const { return mMaxDigDepth; }
	int maxMines() const { return mMaxMines; }

	bool mouseHovering() const { return mMouseInArea; }

	MouseCallback& mouseEnter() { return mMouseEnterCallback; }
	MouseCallback& mouseExit() { return mMouseExitCallback; }

	void update();

protected:
	bool pointInCircle(NAS2D::Point<int> point);
	void onMouseMove(int x, int y, int rX, int rY);

private:
	Planet() = delete;
	Planet(const Planet&) = delete;
	Planet& operator=(const Planet&) = delete;

private:
	int mTick = 0;

	int mMaxMines = 0;
	int mMaxDigDepth = 0;

	NAS2D::Image mImage;
	NAS2D::Point<int> mPosition;

	PlanetType mType = PlanetType::PLANET_TYPE_NONE;

	MouseCallback mMouseEnterCallback;
	MouseCallback mMouseExitCallback;

	bool mMouseInArea = false;

	NAS2D::Timer mTimer;
};
