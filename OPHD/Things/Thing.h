#pragma once

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/Resource/Sprite.h>

#include <string>


/**
 * Thing interface
 *
 * Does not own it's own coordinates.
 * Owner is responsible for drawing at correct location.
 */
class Thing
{
public:
	using DieSignal = NAS2D::Signal<Thing*>;

public:
	Thing(const std::string& name, const std::string& spritePath, const std::string& initialAction);
	Thing(const Thing& thing) = delete;
	Thing& operator=(const Thing& thing) = delete;

	virtual ~Thing() = default;

	virtual void update() = 0;

	const std::string& name() const;

	NAS2D::Sprite& sprite();

	virtual void die();
	bool dead() const;

	DieSignal::Source& onDie();

private:
	std::string mName;
	NAS2D::Sprite mSprite;

	bool mIsDead = false;

	DieSignal mDieSignal;
};
