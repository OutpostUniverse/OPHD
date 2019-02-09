#pragma once

#include "Robot.h"

#include "../../Constants/Strings.h"

class Robodozer: public Robot
{
public:
	Robodozer(): Robot(constants::ROBODOZER, "robots/robodozer.sprite")
	{
		sprite().play("running");
	}

	virtual ~Robodozer() {}

	void tileIndex(size_t index) { mTileIndex = index; }
	size_t tileIndex() const { return mTileIndex; }

	void update() { updateTask(); }

private:
	size_t mTileIndex = 0;
};
