#pragma once

#include <NAS2D/Resource/Sprite.h>

#include <string>


/**
 * MapObject interface
 *
 * Does not own it's own coordinates.
 * Owner is responsible for drawing at correct location.
 */
class MapObject
{
public:
	MapObject(const std::string& spritePath, const std::string& initialAction);
	MapObject(const MapObject& thing) = delete;
	MapObject& operator=(const MapObject& thing) = delete;
	virtual ~MapObject() = default;

	virtual const std::string& name() const = 0;
	virtual void update() = 0;

	virtual void updateAnimation();
	virtual void draw(NAS2D::Point<int> position) const;
	NAS2D::Sprite& sprite();

protected:
	NAS2D::Sprite mSprite;
};
