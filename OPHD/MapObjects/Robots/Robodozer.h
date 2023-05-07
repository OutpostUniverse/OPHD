#pragma once

#include "../Robot.h"

#include "../../Constants/Strings.h"

class Robodozer : public Robot
{
public:
	Robodozer(): Robot(constants::Robodozer, "robots/robodozer.sprite", Robot::Type::Dozer)
	{
	}

	void tileIndex(std::size_t index) { mTileIndex = index; }
	std::size_t tileIndex() const { return mTileIndex; }

private:
	std::size_t mTileIndex = 0;
};
