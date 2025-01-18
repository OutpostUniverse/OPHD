#pragma once

#include <NAS2D/Signal/Signal.h>
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
	using DieSignal = NAS2D::Signal<MapObject*>;

public:
	MapObject(const std::string& name, const std::string& spritePath, const std::string& initialAction);
	MapObject(const MapObject& thing) = delete;
	MapObject& operator=(const MapObject& thing) = delete;
	virtual ~MapObject() = default;

	virtual void update() = 0;
	NAS2D::Sprite& sprite();
	const std::string& name() const;

	bool isDead() const;
	virtual void die();
	DieSignal::Source& onDie();

private:
	std::string mName;
	NAS2D::Sprite mSprite;
	DieSignal mDieSignal;
	bool mIsDead = false;
};
