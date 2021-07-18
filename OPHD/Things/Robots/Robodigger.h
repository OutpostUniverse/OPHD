#pragma once

#include "Robot.h"

#include "../../Common.h"
#include "../../Constants/Strings.h"


class Robodigger : public Robot
{
public:
	Robodigger() :
		Robot(constants::Robodigger, "robots/robodigger.sprite", Robot::Type::Digger),
		mDirection(Direction::Down)
	{
	}

	void direction(Direction dir) { mDirection = dir; }
	Direction direction() const { return mDirection; }

	void update() override { updateTask(); }

	NAS2D::Dictionary getDataDict() const override
	{
		auto dictionary = Robot::getDataDict();
		dictionary.set("direction", static_cast<int>(mDirection));
		return dictionary;
	}

private:

	Direction mDirection;
};
