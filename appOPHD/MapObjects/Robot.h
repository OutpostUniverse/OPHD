#pragma once

#include "RobotTypeIndex.h"

#include <libOPHD/MapObjects/MapObject.h>

#include <NAS2D/Signal/Delegate.h>


namespace NAS2D
{
	class Dictionary;
}

class Tile;
class TileMap;


class Robot : public MapObject
{
public:
	using TaskCompleteDelegate = NAS2D::Delegate<void(Robot&)>;

public:
	Robot(const std::string& name, const std::string& spritePath, RobotTypeIndex robotTypeIndex);

	const std::string& name() const override;

	bool isDead() const;
	virtual void die();

	virtual void processTurn(TileMap& tileMap);

	virtual void startTask(Tile& tile);
	void startTask(int turns);

	virtual void abortTask(Tile& /*tile*/) {}

	void fuelCellAge(int age) { mFuelCellAge = age; }
	int fuelCellAge() const { return mFuelCellAge; }
	int turnsToCompleteTask() const { return mTurnsToCompleteTask; }

	bool selfDestruct() const { return mSelfDestruct; }
	void selfDestruct(bool value) { mSelfDestruct = value; }

	bool idle() const { return turnsToCompleteTask() == 0; }
	void cancelTask() { mCancelTask = true; }
	bool taskCanceled() const { return mCancelTask; }
	void reset() { mCancelTask = false; }

	RobotTypeIndex type() const { return mType; }

	void taskCompleteHandler(TaskCompleteDelegate newTaskCompleteHandler);

	virtual NAS2D::Dictionary getDataDict() const;

protected:
	virtual void onTaskComplete(TileMap& tileMap) = 0;

private:
	const std::string& mName;
	const RobotTypeIndex mType;
	int mFuelCellAge = 0;
	int mTurnsToCompleteTask = 0;

	bool mIsDead = false;
	bool mSelfDestruct = false;
	bool mCancelTask{false};

	TaskCompleteDelegate mTaskCompleteHandler;
};
