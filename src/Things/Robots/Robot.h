#pragma once

#include "../Thing.h"


class Robot: public Thing
{
public:
	typedef NAS2D::Signals::Signal0<void> Callback;
	typedef NAS2D::Signals::Signal1<Robot*> TaskCallback;

	Robot(const std::string& name, const std::string& sprite_path);

	virtual ~Robot();

	void startTask(int turns);

	void fuelCellAge(int age) { mFuelCellAge = age; }
	int fuelCellAge() const { return mFuelCellAge; }
	int turnsToCompleteTask() const { return mTurnsToCompleteTask; }

	bool selfDestruct() const { return mSelfDestruct; }
	void seldDestruct(bool _b) { mSelfDestruct = _b; }

	bool idle() const { return turnsToCompleteTask() == 0; }

	TaskCallback& taskComplete() { return mTaskCompleteCallback; }
	Callback& selfDestruct() { return mSelfDestructCallback; }

protected:

	void incrementFuelCellAge() { mFuelCellAge++; }
	void updateTask();

private:

	int				mFuelCellAge;
	int				mTurnsToCompleteTask;
	bool			mSelfDestruct;

	TaskCallback	mTaskCompleteCallback;
	Callback		mSelfDestructCallback;
};
