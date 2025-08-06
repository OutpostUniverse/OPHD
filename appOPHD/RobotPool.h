#pragma once

#include <cstddef>
#include <vector>
#include <list>


enum class RobotTypeIndex;
enum class Direction;
class Robot;
class Robodigger;
class Robodozer;
class Robominer;
class Tile;
class StructureManager;

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

public:
	RobotPool(const StructureManager& structureManager);
	~RobotPool();

	Robot& addRobot(RobotTypeIndex robotTypeIndex);

	bool robotAvailable(RobotTypeIndex robotTypeIndex) const;
	std::size_t getAvailableCount(RobotTypeIndex robotTypeIndex) const;

	bool isControlCapacityAvailable() const { return mRobotControlCount < mRobotControlMax; }
	bool commandCapacityAvailable() { return mRobots.size() < mRobotControlMax; }
	void update();

	DiggerList& diggers();
	DozerList& dozers();
	MinerList& miners();

	const DiggerList& diggers() const;
	const DozerList& dozers() const;
	const MinerList& miners() const;

	void removeDeployedRobots();
	void clear();
	void erase(Robot* robot);
	void deploy(Robot& robot, Tile& tile);

	void deployDigger(Tile& tile, Direction direction);
	void deployDozer(Tile& tile);
	void deployMiner(Tile& tile);

	std::size_t robotControlMax() const { return mRobotControlMax; }
	std::size_t currentControlCount() const { return mRobotControlCount; }

	const RobotList& robots() const { return mRobots; }
	RobotList& deployedRobots() { return mDeployedRobots; }

	NAS2D::Xml::XmlElement* writeRobots();

protected:
	Robodigger& getDigger();
	Robodozer& getDozer();
	Robominer& getMiner();

private:
	const StructureManager& mStructureManager;

	DiggerList mDiggers;
	DozerList mDozers;
	MinerList mMiners;

	RobotList mRobots; // List of all robots by pointer to base class
	RobotList mDeployedRobots;

	std::size_t mRobotControlMax = 0;
	std::size_t mRobotControlCount = 0;
};
