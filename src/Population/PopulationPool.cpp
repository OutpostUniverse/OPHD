#include "PopulationPool.h"

#include "NAS2D/NAS2D.h"

int PopulationPool::pullWorkers(int _i)
{
	if(_i <= mWorkers - mWorkersOccupied)
	{
		mWorkersOccupied += _i;
		return _i;
	}
	else if(_i >= mWorkers - mWorkersOccupied)
	{
		int diff = _i - (mWorkers - mWorkersOccupied);
		if(diff <= mScientists - mScientistsOccupied)
		{
			mWorkersOccupied = _i - diff;
			mScientistsOccupied += diff;
			mScientistsAsWorkers += diff;
		}

		return _i;
	}
	else
		return 0;
}


int PopulationPool::pullScientists(int _i)
{
	if(_i <= mScientists - mScientistsOccupied)
	{
		mScientistsOccupied += _i;
		return _i;
	}
	else
		return 0;
}


void PopulationPool::releaseWorkers(int _i)
{
	if(mScientistsAsWorkers > 0)
	{
		if(_i >= mScientistsAsWorkers)
		{
			mScientistsOccupied -= mScientistsAsWorkers;
			mWorkersOccupied -= _i - mScientistsAsWorkers;
			mScientistsAsWorkers = 0;
		}
		else
		{
			mScientistsOccupied -= _i;
			mScientistsAsWorkers -= _i;
		}
	}
	else
		mWorkersOccupied -= _i;
}


void PopulationPool::releaseScientists(int _i)
{
	mScientistsOccupied -= _i;
}

bool PopulationPool::enoughWorkers(int _i)
{
	if(_i < mWorkers - mWorkersOccupied)
		return true;
	
	return false;
}


bool PopulationPool::enoughScientists(int _i)
{
	if(_i < mScientists - mScientistsOccupied)
		return true;

	return false;
}


/**
 * Overly simplistic
 */
void PopulationPool::killPerson()
{
	int x = NAS2D::Random::ranged_integer(0, 1000);

	if(x < 250)
		--mChildren;
	if(x < 500)
		--mStudents;
	if(x < 750)
		--mWorkers;
	if(x <= 1000)
		--mScientists;	
}