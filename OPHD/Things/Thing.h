#pragma once

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/Resource/Sprite.h>

#include <string>


/**
 * Class implementing a Thing interface.
 */
class Thing
{
public:
	using DieSignal = NAS2D::Signal<Thing*>;

public:
	Thing(const std::string& name, const std::string& spritePath, const std::string& initialAction) :
		mName(name),
		mSprite(spritePath, initialAction)
	{}

	virtual ~Thing()
	{
	}

	virtual void update() = 0;

	const std::string& name() const { return mName; }

	/**
	 * Gets a reference to the Sprite used to represent this Thing.
	 * 
	 * ASSUMPTION: The code that actually draws the map including Things
	 * should be responsible for drawing and updating the Thing as well.
	 * Making the thing responsible for drawing itself needlessly complicates
	 * the code as it requires that the Thing have screen positional
	 * information included in it.
	 */
	NAS2D::Sprite& sprite() { return mSprite; }

	virtual void die() { mIsDead = true; mDieSignal(this); }
	bool dead() const { return mIsDead; }

	DieSignal::Source& onDie() { return mDieSignal; }

private:
	// No default copy constructor, or copy operator
	// Calling these should result in an error
	Thing(const Thing& thing) = delete;
	Thing& operator=(const Thing& thing) = delete;

private:
	std::string mName; /**< Name of the Thing. */
	NAS2D::Sprite mSprite; /**< Sprite used to represent the Thing. */

	bool mIsDead = false;/**< Thing is dead and should be cleaned up. */

	DieSignal mDieSignal;
};
