#ifndef __ROBOT__
#define __ROBOT__

#include "../Thing.h"


class Robot: public Thing
{
public:
	typedef Gallant::Signal0<void> Callback;
	typedef Gallant::Signal1<Robot*> TaskCallback;

	Robot(const std::string& name, const std::string& sprite_path):	Thing(name, sprite_path),
																	mFuelCellAge(0),
																	mTurnsToCompleteTask(0),
																	mSelfDestruct(false)
	{
	}

	virtual ~Robot()
	{}

	void startTask(int turns) { turnsToCompleteTask(turns); }

	bool selfDestruct() const { return mSelfDestruct; }
	void seldDestruct(bool _d) { mSelfDestruct = _d; }

	TaskCallback& taskComplete() { return mTaskCompleteCallback; }
	Callback& selfDestruct() { return mSelfDestructCallback; }

	bool idle() const { return mTurnsToCompleteTask == 0; }

protected:

	int fuelCellAge() const { return mFuelCellAge; }
	int incrementFuelCellAge() { mFuelCellAge++; }

	void turnsToCompleteTask(int _i)
	{
		if (_i < 1)
			mTurnsToCompleteTask = 1;
		else
			mTurnsToCompleteTask = _i;
	}

	void updateTask()
	{
		if(mSelfDestruct)
		{
			die();
			mSelfDestructCallback();
		}

		mTurnsToCompleteTask--;

		if(mTurnsToCompleteTask == 0)
			mTaskCompleteCallback(this);

		mFuelCellAge++;

		if(mFuelCellAge == 200)
			die();
	}

private:

	int				mFuelCellAge;
	int				mTurnsToCompleteTask;
	bool			mSelfDestruct;

	TaskCallback	mTaskCompleteCallback;
	Callback		mSelfDestructCallback;
};


#endif