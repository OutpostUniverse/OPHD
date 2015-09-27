#ifndef __PERSON__
#define __PERSON__

/**
 * Represents a Person in a population.
 * 
 * A Person can only 'live' so long. Factors such as Morale
 * can drastically shorten the 'life' of a person.
 * 
 * \todo	Decide if I really want the population model to be
 *			sophisticated enough that each person in the simulation
 *			has their own personal object. It may be overkill for
 *			what Outpost does.
 */
class Person
{
public:

	enum PersonType
	{
		CHILD,
		STUDENT,
		WORKER,
		SCIENTIST
	};

	Person(int age):	mAge(age),
						mMaxAge(3900)
	{}

	Person():		mAge(0),
					mMaxAge(3900)
	{}

	~Person()
	{}

	bool dead() const { return mAge >= mMaxAge; }

	void upate()
	{
		++mAge;
	}

protected:

	void determineType()
	{

	}

private:

	int		mAge;					/**< Current age of the Person. */
	int		mMaxAge;				/**< Maximum life span of the Person. */

	int		mUniversityTime;		/**< Time spent in a university. */

	int		mMorale;				/**< Morale level of the Person. Can be between 0 - 1000 */

	bool	mDead;					/**< Flag indicating that the Person is dead. */
};

#endif