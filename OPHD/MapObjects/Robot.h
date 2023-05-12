#pragma once

#include "MapObject.h"

#include <NAS2D/Dictionary.h>


class Tile;


class Robot : public MapObject
{
public:
	enum class Type
	{
		Digger,
		Dozer,
		Miner,

		None
	};

	using TaskSignal = NAS2D::Signal<Robot*>;

public:
	Robot(const std::string&, const std::string&, Type);
	Robot(const std::string&, const std::string&, const std::string&, Type);

	void update() override;

	virtual void startTask(Tile& tile);
	void startTask(int turns);

	virtual void abortTask(Tile& /*tile*/) {}

	void fuelCellAge(int age) { mFuelCellAge = age; }
	int fuelCellAge() const { return mFuelCellAge; }
	int turnsToCompleteTask() const { return mTurnsToCompleteTask; }

	bool selfDestruct() const { return mSelfDestruct; }
	void seldDestruct(bool value) { mSelfDestruct = value; }

	bool idle() const { return turnsToCompleteTask() == 0; }
	void cancelTask() { mCancelTask = true; }
	bool taskCanceled() const { return mCancelTask; }
	void reset() { mCancelTask = false; }

	Type type() const { return mType; }

	TaskSignal::Source& taskComplete() { return mTaskCompleteSignal; }

	virtual NAS2D::Dictionary getDataDict() const;

protected:
	void incrementFuelCellAge() { mFuelCellAge++; }

private:
	int mFuelCellAge = 0;
	int mTurnsToCompleteTask = 0;

	bool mSelfDestruct = false;
	bool mCancelTask{false};

	Type mType{Type::None};

	TaskSignal mTaskCompleteSignal;
};
