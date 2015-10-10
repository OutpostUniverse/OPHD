#ifndef __ROBOT__
#define __ROBOT__

#include "../Thing.h"


class Robot: public Thing
{
public:
	typedef Gallant::Signal0<void> Callback;
	typedef Gallant::Signal1<Robot*> TaskCallback;

	Robot(const std::string& name, const std::string& sprite_path);

	virtual ~Robot();

	void startTask(int turns);

	bool selfDestruct() const { return mSelfDestruct; }
	void seldDestruct(bool _b) { mSelfDestruct = _b; }

	TaskCallback& taskComplete() { return mTaskCompleteCallback; }
	Callback& selfDestruct() { return mSelfDestructCallback; }

	bool idle() const { return mTurnsToCompleteTask == 0; }

protected:

	int fuelCellAge() const { return mFuelCellAge; }
	int incrementFuelCellAge() { mFuelCellAge++; }

	void updateTask();

private:

	int				mFuelCellAge;
	int				mTurnsToCompleteTask;
	bool			mSelfDestruct;

	TaskCallback	mTaskCompleteCallback;
	Callback		mSelfDestructCallback;
};


#endif