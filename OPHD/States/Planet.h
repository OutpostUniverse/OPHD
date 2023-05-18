#pragma once

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/Timer.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Resource/Image.h>

#include <string>
#include <vector>


class Planet
{
public:
	enum class PlanetType
	{
		Mercury,
		Mars,
		Ganymede,
	};

	enum class Hostility
	{
		Low,
		Medium,
		High
	};

	struct Attributes
	{
		PlanetType type{PlanetType::Mercury};
		std::string imagePath;
		Hostility hostility{Hostility::Low};
		int maxDepth = 0;
		std::size_t maxMines = 0;
		std::string mapImagePath;
		std::string tilesetPath;
		std::string name;

		/* Mean distance from star in astronomical units (AU) */
		float meanSolarDistance = 0;

		std::string description;
	};

public:
	using MouseSignal = NAS2D::Signal<>;

public:
	Planet(const Attributes& attributes);
	~Planet();

	NAS2D::Point<int> position() const { return mPosition; }
	void position(const NAS2D::Point<int>& point) { mPosition = point; }

	const Attributes& attributes() const { return mAttributes; }

	bool mouseHovering() const { return mMouseInArea; }

	MouseSignal::Source& mouseEnter() { return mMouseEnterSignal; }
	MouseSignal::Source& mouseExit() { return mMouseExitSignal; }

	void update();

protected:
	bool pointInCircle(NAS2D::Point<int> point);
	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

private:
	Planet() = delete;
	Planet(const Planet&) = delete;
	Planet& operator=(const Planet&) = delete;

private:
	int mTick = 0;

	Attributes mAttributes;

	const NAS2D::Image mImage;
	NAS2D::Point<int> mPosition;

	MouseSignal mMouseEnterSignal;
	MouseSignal mMouseExitSignal;

	bool mMouseInArea = false;

	NAS2D::Timer mTimer;
};

std::vector<Planet::Attributes> parsePlanetAttributes();
