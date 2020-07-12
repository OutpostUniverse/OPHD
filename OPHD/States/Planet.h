#pragma once

#include <NAS2D/Signal.h>
#include <NAS2D/Timer.h>
#include <NAS2D/Renderer/Point.h>
#include <NAS2D/Resources/Image.h>

#include <cmath>


class Planet
{
public:
	enum class PlanetType
	{
		None,

		Mercury,
		Mars,
		Ganymede,

		Count
	};

	enum class Hostility
	{
		None,

		Low,
		Medium,
		High
	};

public:
	using MouseCallback = NAS2D::Signals::Signal<>;

public:
	Planet(PlanetType type);
	~Planet();

	PlanetType type() const { return mType; }

	NAS2D::Point<int> position() const { return mPosition; }
	void position(const NAS2D::Point<int>& point) { mPosition = point; }
	void position(int x, int y) { mPosition = {x, y}; }

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

	PlanetType mType = PlanetType::None;

	MouseCallback mMouseEnterCallback;
	MouseCallback mMouseExitCallback;

	bool mMouseInArea = false;

	NAS2D::Timer mTimer;
};
