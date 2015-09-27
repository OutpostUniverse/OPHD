#ifndef __POPULATION_POOL__
#define __POPULATION_POOL__

/**
 * Represents a pool of Person's.
 */
class PopulationPool
{
public:
	PopulationPool():	mChildren(0),
						mStudents(0),
						mWorkers(0),
						mScientists(0),
						mScientistsAsWorkers(0),
						mWorkersOccupied(0),
						mScientistsOccupied(0)

	{}

	~PopulationPool()
	{}

	int pullWorkers(int _i);
	int pullScientists(int _i);

	void releaseWorkers(int _i);
	void releaseScientists(int _i);

	bool enoughWorkers(int _i);
	bool enoughScientists(int _i);

	int children() const { return mChildren; }
	int students() const { return mStudents; }
	int workers() const { return mWorkers; }
	int scientists() const { return mScientists; }
	int totalPopulation() const { return mChildren + mStudents + mWorkers + mScientists; }

	void addChildren(int _i) { mChildren += _i; }
	void addStudents(int _i) { mStudents += _i; }
	void addWorkers(int _i) { mWorkers += _i; }
	void addScientists(int _i) { mScientists += _i; }

	void killPerson();

protected:
private:

	int		mChildren;
	int		mStudents;
	int		mWorkers;
	int		mScientists;

	int		mScientistsAsWorkers;
	int		mWorkersOccupied;
	int		mScientistsOccupied;
};


#endif