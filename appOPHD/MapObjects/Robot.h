#pragma once

#include "RobotTypeIndex.h"

#include <libOPHD/MapObjects/MapObject.h>

#include <NAS2D/Signal/Delegate.h>


namespace NAS2D
{
	class Dictionary;
}

struct RobotType;
struct MapCoordinate;
class Tile;
class TileMap;
class StructureManager;


class Robot : public MapObject
{
public:
	using TaskCompleteDelegate = NAS2D::Delegate<void(Robot&)>;

	static const RobotType& robotType(RobotTypeIndex robotTypeIndex);

public:
	Robot(RobotTypeIndex robotTypeIndex);
	Robot(RobotTypeIndex robotTypeIndex, const RobotType& robotType);

	const RobotType& robotType() const;
	const std::string& name() const override;

	bool isDead() const;
	virtual void die();

	virtual void processTurn(TileMap& tileMap, StructureManager& structureManager);

	virtual void startTask(Tile& tile);
	void startTask(Tile& tile, int turns);

	virtual void abortTask();

	bool isPlaced() const;
	Tile& tile();
	const Tile& tile() const;
	MapCoordinate mapCoordinate() const;

	void fuelCellAge(int age) { mFuelCellAge = age; }
	int fuelCellAge() const { return mFuelCellAge; }
	int turnsToCompleteTask() const { return mTurnsToCompleteTask; }

	bool selfDestruct() const { return mSelfDestruct; }
	void selfDestruct(bool value) { mSelfDestruct = value; }

	bool idle() const { return turnsToCompleteTask() == 0; }
	void cancelTask() { mCancelTask = true; }
	bool taskCanceled() const { return mCancelTask; }
	void reset() { mCancelTask = false; }

	RobotTypeIndex type() const { return mRobotTypeIndex; }

	void taskCompleteHandler(TaskCompleteDelegate newTaskCompleteHandler);

	virtual NAS2D::Dictionary getDataDict() const;

protected:
	virtual void onTaskComplete(TileMap& tileMap, StructureManager& structureManager) = 0;

private:
	const RobotType& mRobotType;
	const RobotTypeIndex mRobotTypeIndex;
	int mFuelCellAge = 0;
	int mTurnsToCompleteTask = 0;
	Tile* mTile = nullptr;

	bool mIsDead = false;
	bool mSelfDestruct = false;
	bool mCancelTask{false};

	TaskCompleteDelegate mTaskCompleteHandler;
};
