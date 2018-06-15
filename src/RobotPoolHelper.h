/**
 * Templete helper functions used exclusively by RobotPool.
 * 
 * This header and its functions should not be used anywhere else,
 * these are designed specifically to help improve code readability
 * and maintainability of the RobotPool class.
 */
#pragma once

template <class T>
void clearRobots(T& t)
{
	for (auto robot : t)
	{
		delete robot;
	}

	t.clear();
}


template <class T>
Robot* getRobot(const T& t)
{
	for (auto robot : t)
	{
		if (robot->idle()) { return robot; }
	}
	return nullptr;
}


template <class T>
int getIdleCount(const T& t)
{
	int count = 0;
	for (auto robot : t)
	{
		if (robot->idle()) { ++count; }
	}

	return count;
}


template <class T>
void updateRobotControlCount(const T& t, uint32_t& controlCounter)
{
	for (auto robot : t)
	{
		if (!robot->idle() && !robot->dead()) { ++controlCounter; }
	}
}
