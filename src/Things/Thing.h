#pragma once

#include "NAS2D/NAS2D.h"

class Tile;

using namespace NAS2D;

#include <iostream>
#include <string>

class Thing
{
public:
	typedef NAS2D::Signals::Signal1<Thing*> DieCallback;

	Thing(const std::string& name, const std::string& sprite_path):	mName(name),
																	mSprite(sprite_path),
																	mIsDead(false)
	{}

	Thing():	mName("Unknown")
	{}

	virtual ~Thing()
	{
		#ifdef _DEBUG
		std::cout << mName << ": He's dead Jim!" << std::endl;
		#endif
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
	Sprite& sprite() { return mSprite; }

	virtual void die() { mIsDead = true; mDieCallback(this); }
	bool dead() const { return mIsDead; }

	DieCallback& onDie() { return mDieCallback; }

protected:

private:

	Thing(const Thing& thing) {} // Explicitely defined private
	Thing& operator=(const Thing& thing) {} // Explicitely defined private

	std::string		mName;			/**< Name of the Thing. */
	Sprite			mSprite;		/**< Sprite used to represent the Thing. */

	bool			mIsDead;		/**< Thing is dead and should be cleaned up. */

	DieCallback		mDieCallback;	/**<  */
};
