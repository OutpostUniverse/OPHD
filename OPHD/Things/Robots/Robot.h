#pragma once

#include "../Thing.h"


class Robot: public Thing
{
public:
	using Callback = NAS2D::Signals::Signal<>;
	using TaskCallback = NAS2D::Signals::Signal<Robot*>;

public:
	Robot(const std::string& name, const std::string& sprite_path);

	void startTask(int turns);

	void fuelCellAge(int age) { mFuelCellAge = age; }
	int fuelCellAge() const { return mFuelCellAge; }
	int turnsToCompleteTask() const { return mTurnsToCompleteTask; }

	bool selfDestruct() const { return mSelfDestruct; }
	void seldDestruct(bool value) { mSelfDestruct = value; }

	bool idle() const { return turnsToCompleteTask() == 0; }

	TaskCallback& taskComplete() { return mTaskCompleteCallback; }
	Callback& selfDestruct() { return mSelfDestructCallback; }

	void id(int newId) { mId = newId; }
	int id() const { return mId; }

protected:
	void incrementFuelCellAge() { mFuelCellAge++; }
	void updateTask();

private:
	int				mId = 0;
	int				mFuelCellAge = 0;
	int				mTurnsToCompleteTask = 0;

	bool			mSelfDestruct = false;

	TaskCallback	mTaskCompleteCallback;
	Callback		mSelfDestructCallback;
};
