#pragma once


#include "MapObjects/Robots.h"

#include <cstddef>
#include <vector>
#include <list>
#include <map>


class Tile;
using RobotList = std::vector<Robot*>;

namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}
}


class RobotPool
{
public:
	using DiggerList = std::list<Robodigger>;
	using DozerList = std::list<Robodozer>;
	using MinerList = std::list<Robominer>;
	using RobotTileTable = std::map<Robot*, Tile*>;

public:
	RobotPool();
	~RobotPool();

	Robot& addRobot(RobotType type);

	Robodigger& getDigger();
	Robodozer& getDozer();
	Robominer& getMiner();

	bool robotAvailable(RobotType type) const;
	std::size_t getAvailableCount(RobotType type) const;

	bool isControlCapacityAvailable() const { return mRobotControlCount < mRobotControlMax; }
	bool commandCapacityAvailable() { return mRobots.size() < mRobotControlMax; }
	void update();

	DiggerList& diggers() { return mDiggers; }
	DozerList& dozers() { return mDozers; }
	MinerList& miners() { return mMiners; }

	const DiggerList& diggers() const { return mDiggers; }
	const DozerList& dozers() const { return mDozers; }
	const MinerList& miners() const { return mMiners; }

	void clear();
	void erase(Robot* robot);
	void insertRobotIntoTable(RobotTileTable& robotMap, Robot& robot, Tile& tile);

	std::size_t robotControlMax() const { return mRobotControlMax; }
	std::size_t currentControlCount() const { return mRobotControlCount; }

	const RobotList& robots() const { return mRobots; }

	NAS2D::Xml::XmlElement* writeRobots(RobotTileTable& robotMap);

private:
	DiggerList mDiggers;
	DozerList mDozers;
	MinerList mMiners;

	RobotList mRobots; // List of all robots by pointer to base class

	std::size_t mRobotControlMax = 0;
	std::size_t mRobotControlCount = 0;
};
