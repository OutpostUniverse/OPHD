#pragma once

/**
 * Templete helper functions used exclusively by RobotPool.
 * 
 * This header and its functions should not be used anywhere else,
 * these are designed specifically to help improve code readability
 * and maintainability of the RobotPool class.
 */

#include <cstddef>
#include <stdexcept>


template <class T>
void clearRobots(T& list)
{
	for (const auto& robot : list)
	{
		delete robot;
	}

	list.clear();
}


template <class T>
void eraseRobot(T& list, Robot* robot)
{
	auto it = find(list.begin(), list.end(), robot);
	if (it != list.end())
	{
		list.erase(it);
	}
}


template <class T>
typename T::value_type getIdleRobotOrNull(const T& list)
{
	for (const auto& robot : list)
	{
		if (robot->idle()) { return robot; }
	}
	return nullptr;
}


template <class T>
auto& getIdleRobot(const T& list)
{
	auto* robot = getIdleRobotOrNull(list);
	if (robot == nullptr)
	{
		throw std::runtime_error("Failed to get an idle robot");
	}
	return *robot;
}


template <class T>
std::size_t getIdleCount(const T& list)
{
	std::size_t count = 0;
	for (const auto& robot : list)
	{
		if (robot->idle()) { ++count; }
	}

	return count;
}


template <class T>
std::size_t robotControlCount(const T& list)
{
	std::size_t controlCounter{0};
	for (const auto& robot : list)
	{
		if (!robot->idle() && !robot->isDead()) { ++controlCounter; }
	}
	return controlCounter;
}
