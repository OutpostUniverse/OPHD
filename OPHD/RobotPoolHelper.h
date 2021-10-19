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
Robot* getRobotOrNull(const T& t)
{
	for (auto robot : t)
	{
		if (robot->idle()) { return robot; }
	}
	return nullptr;
}


template <class T>
Robot* getRobot(const T& list)
{
	auto* robot = getRobotOrNull(list);
	if (robot == nullptr)
	{
		throw std::runtime_error("Failed to get an idle robot");
	}
	return robot;
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


template <class T>
void eraseRobot(T& t, Robot* robot)
{
	auto it = find(t.begin(), t.end(), robot);
	if (it != t.end())
	{
		t.erase(it);
	}
}
