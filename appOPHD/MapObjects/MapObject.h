#pragma once

#include <NAS2D/Resource/Sprite.h>

#include <string>


/**
 * MapObject interface
 *
 * Does not own it's own screen coordinates.
 * Owner is responsible for drawing at correct screen location.
 */
class MapObject
{
public:
	MapObject(const std::string& spritePath, const std::string& initialAction);
	MapObject(const MapObject& mapObject) = delete;
	MapObject& operator=(const MapObject& mapObject) = delete;
	virtual ~MapObject() = default;

	virtual const std::string& name() const = 0;

	virtual void updateAnimation();
	virtual void draw(NAS2D::Point<int> position) const;

protected:
	NAS2D::Sprite mSprite;
};
