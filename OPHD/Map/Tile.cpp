#include "Tile.h"

#include "../Mine.h"
#include "../Things/Robots/Robot.h"
#include "../Things/Structures/Structure.h"

#include <cmath>


std::map<Tile::Overlay, NAS2D::Color> OverlayColorTable =
{
	{Tile::Overlay::None, NAS2D::Color::Normal},
	{Tile::Overlay::Communications, {125, 200, 255}},
	{Tile::Overlay::Connectedness, NAS2D::Color::Green},
	{Tile::Overlay::TruckingRoutes, NAS2D::Color::Orange},
	{Tile::Overlay::Police, NAS2D::Color::Red}
};

std::map<Tile::Overlay, NAS2D::Color> OverlayHighlightColorTable =
{
	{Tile::Overlay::None, NAS2D::Color{125, 200, 255}},
	{Tile::Overlay::Communications, {100, 180, 230}},
	{Tile::Overlay::Connectedness, NAS2D::Color{71, 224, 146}},
	{Tile::Overlay::TruckingRoutes, NAS2D::Color{125, 200, 255}},
	{Tile::Overlay::Police, NAS2D::Color{100, 180, 230}}
};


const NAS2D::Color& overlayColor(Tile::Overlay overlay, bool isHighlighted)
{
	if (isHighlighted)
	{
		return overlayHighlightColor(overlay);
	}

	return overlayColor(overlay);
}


const NAS2D::Color& overlayColor(Tile::Overlay overlay)
{
	return OverlayColorTable.at(overlay);
}


const NAS2D::Color& overlayHighlightColor(Tile::Overlay overlay)
{
	return OverlayHighlightColorTable.at(overlay);
}


Tile::Tile(NAS2D::Point<int> position, int depth, TerrainType index) :
	mIndex{index},
	mPosition{position, depth}
{}


Tile::Tile(Tile&& other) noexcept :
	mIndex{other.mIndex},
	mPosition{other.mPosition},
	mThing{other.mThing},
	mMine{other.mMine},
	mOverlay{other.mOverlay},
	mExcavated{other.mExcavated}
{
	other.mThing = nullptr;
	other.mMine = nullptr;
}


Tile& Tile::operator=(Tile&& other) noexcept
{
	mIndex = other.mIndex;
	mPosition = other.mPosition;
	mThing = other.mThing;
	mMine = other.mMine;
	mOverlay = other.mOverlay;
	mExcavated = other.mExcavated;

	other.mThing = nullptr;
	other.mMine = nullptr;

	return *this;
}


Tile::~Tile()
{
	delete mMine;
	delete mThing;
}


/**
 * Adds a new Thing to the tile.
 *
 * \param	thing		Pointer to a Thing.
 */
void Tile::pushThing(Thing* thing)
{
	if (mThing)
	{
		deleteThing();
	}

	mThing = thing;
}


/**
 * Clears a Thing from the Tile.
 *
 * \note	Garbage collects the Thing. Deletes and frees memory.
 */
void Tile::deleteThing()
{
	delete mThing;
	removeThing();
}


/**
 * Removes a Thing from the Tile.
 *
 * \note	Does NOT delete or free memory. Simply clears the pointer, not the memory.
 */
void Tile::removeThing()
{
	mThing = nullptr;
}


void Tile::pushMine(Mine* mine)
{
	delete mMine;
	mMine = mine;
}


Structure* Tile::structure() const
{
	return dynamic_cast<Structure*>(thing());
}


Robot* Tile::robot() const
{
	return dynamic_cast<Robot*>(thing());
}
