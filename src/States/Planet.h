#pragma once

#include <cmath>

#include <NAS2D/NAS2D.h>

#include "../Constants.h"


enum PlanetType
{
	PLANET_TYPE_NONE,
	PLANET_TYPE_MERCURY,
	PLANET_TYPE_MARS,
	PLANET_TYPE_GANYMEDE
};


class Planet
{
public:
	typedef NAS2D::Signals::Signal0<void> MouseCallback;

public:
	Planet(PlanetType type) : mTick(0), mMaxMines(0), mMaxDigDepth(0), mImage(), mType(type), mMouseInArea(false)
	{
		// Fixme: This should be in a table vs. a giant switch statement.
		switch (mType)
		{
		case PLANET_TYPE_MERCURY:
			mImage = NAS2D::Image(constants::PLANET_TYPE_MERCURY_PATH);
			mMaxMines = 10;
			mMaxDigDepth = 1;
			break;
		case PLANET_TYPE_MARS:
			mImage = NAS2D::Image(constants::PLANET_TYPE_MARS_PATH);
			mMaxMines = 30;
			mMaxDigDepth = 4;
			break;
		case PLANET_TYPE_GANYMEDE:
			mMaxMines = 15;
			mMaxDigDepth = 2;
			mImage = NAS2D::Image(constants::PLANET_TYPE_GANYMEDE_PATH);
			break;
		default:
			throw std::runtime_error("Instantiated Planet class without a valid planet type.");
			break;
		}

		NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().connect(this, &Planet::onMouseMove);
	}


	~Planet()
	{
		NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().disconnect(this, &Planet::onMouseMove);
	}


	PlanetType type() const { return mType; }


	void position(const NAS2D::Point_2d& _pt) { mPosition = _pt; }
	void position(int x, int y) { mPosition(x, y); }

	int x() const { return mPosition.x(); }
	int y() const { return mPosition.y(); }

	int digDepth() const { return mMaxDigDepth; }
	int maxMines() const { return mMaxMines; }

	bool mouseHovering() const { return mMouseInArea; }

	MouseCallback& mouseEnter() { return mMouseEnterCallback; }
	MouseCallback& mouseExit() { return mMouseExitCallback; }

	void update()
	{
		if (mMouseInArea && mTimer.accumulator() >= constants::PLANET_ANIMATION_SPEED)
		{
			mTimer.reset(); // don't care about frame skips.
			++mTick;
		}

		// FIXME:	Table approach would be a lot faster for this instead of using multiplications and modulus operations.
		//			In the limited scope that this class is used it's not really worth it to go through a full implementation
		//			as only a few of these objects will ever be on screen ever and as of 11/1/2015 are only ever used once
		//			during planetary selection at the beginning of the game.
		NAS2D::Utility<NAS2D::Renderer>::get().drawSubImage(mImage, (float)mPosition.x(), (float)mPosition.y(), (float)(mTick % 8 * 128), (float)(((mTick % 64) / 8) * 128), 128.0f, 128.0f);
	}


protected:

	bool pointInArea(int x, int y)
	{
		// Standard point in circle equation. Magic numbers for a circle diameter of 128.
		// Note: assumes all values are always positive.
		if (pow(x - mPosition.x() - 64, 2) + pow(y - mPosition.y() - 64, 2) <= 4096)
			return true;

		return false;
	}

	void onMouseMove(int x, int y, int rX, int rY)
	{
		if (pointInArea(x, y))
		{
			if (!mMouseInArea)
			{
				mMouseInArea = true;
				mMouseEnterCallback();
			}

			mMouseInArea = true;
		}
		else
		{
			if (mMouseInArea)
			{
				mMouseInArea = false;
				mMouseExitCallback();
			}

			mMouseInArea = false;
			mTimer.reset(); // keep accumulator at 0 so frame tick doesn't update while mouse is not hovering over Planet.
		}
	}

private:

	// No default constructor, copy constructor, or copy operator
	// Calling these should result in an error
	Planet() = delete;
	Planet(const Planet&) = delete;
	Planet& operator=(const Planet&) = delete;


	int					mTick;

	int					mMaxMines;
	int					mMaxDigDepth;

	NAS2D::Image		mImage;
	NAS2D::Point_2d		mPosition;

	PlanetType			mType;

	MouseCallback		mMouseEnterCallback;
	MouseCallback		mMouseExitCallback;

	bool				mMouseInArea;

	NAS2D::Timer		mTimer;
};
