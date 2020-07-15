#pragma once

#include <NAS2D/Signal.h>
#include <NAS2D/Timer.h>
#include <NAS2D/Renderer/Point.h>
#include <NAS2D/Resources/Image.h>

#include <cmath>
#include <string>


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

	struct Attributes
	{
		PlanetType type = PlanetType::None;
		std::string imagePath;
		Hostility hostility;
		int maxDepth = 0;
		int maxMines = 0;
		std::string mapImagePath;
		std::string tilesetPath;
	};

public:
	using MouseCallback = NAS2D::Signals::Signal<>;

public:
	Planet(const Attributes& attributes);
	~Planet();

	PlanetType type() const { return mType; }

	NAS2D::Point<int> position() const { return mPosition; }
	void position(const NAS2D::Point<int>& point) { mPosition = point; }
	void position(int x, int y) { mPosition = {x, y}; }

	Hostility hostility() const { return mHostility; }
	int digDepth() const { return mMaxDigDepth; }
	int maxMines() const { return mMaxMines; }
	std::string mapImagePath() const { return mMapImagePath; };
	std::string tilesetPath() const { return mTilesetPath; };

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

	Hostility mHostility;
	int mMaxMines;
	int mMaxDigDepth;
	std::string mMapImagePath;
	std::string mTilesetPath;

	NAS2D::Image mImage;
	NAS2D::Point<int> mPosition;

	PlanetType mType = PlanetType::None;

	MouseCallback mMouseEnterCallback;
	MouseCallback mMouseExitCallback;

	bool mMouseInArea = false;

	NAS2D::Timer mTimer;
};
