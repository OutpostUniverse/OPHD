#include "Population.h"

#include <algorithm>
#include <iostream>

#include <NAS2D/NAS2D.h>


const int CHILD_BEARING_AGE_MIN = 240;			// months, basically 20.
const int CHILD_BEARING_AGE_MAX = 600;			// months, basically 45.

const int CHILD_TO_STUDENT = 120;				// months, basically 10.
const int STUDENT_TO_ADULT = 216;				// months, basically 20.

const int WORKER_TO_RETIRE = 720;				// months, basically 60.
const int SCIENTIST_TO_RETIRE = 780;			// months, basically 65.

const int STUDENT_TO_SCIENTIST_PERCENT = 35;	// Percent of students that train into scientists.

const int BIRTH_CHANCE_JITTER = 9;

const int CHILD_MORTALITY_RATE = 5;				// Percent chance of child dying during a turn.
const int STUDENT_MORTALITY_RATE = 3;			// Percent chance of a student dying during a turn.
const int ADULT_MORTALITY_RATE = 2;				// Percent chance of adult dying during a turn.

const int AVERAGE_RETIRED_DEATH_AGE = 840;		// Avergae age at which people naturally die.

const int DIVISOR = 1000;

const int MORALE_MODIFIER = -2;


/**
 * Calls Person::update() on a Person.
 * 
 * \param	_p	Pointer to a Person.
 * 
 * \note	Built specifically for use with std::for_each()
 */
void update_person(Person* _p)
{
	_p->update();
}


/**
 * Returns a morale modifier value. Ranges between -2 and 2.
 */
inline int morale_modifier(int morale)
{
	return MORALE_MODIFIER + (morale / 200);
}


/**
 * C'tor
 */
Population::Population(): mDeathCount(0), mBirthCount(0), mCurrentMorale(0)
{}


/**
 * D'tor
 */
Population::~Population()
{
	clear();
}


/**
 * Clears entire population and frees all associated resources.
 */
void Population::clear()
{
	// This is sooooooo hacky it's not even funny.
	for (int i = 0; i < ROLE_RETIRED + 1; ++i)
		clearPopulationList(static_cast<PersonRole>(i));
}


/**
 * Clears a specified segment of the population and frees all resources
 * associated with it.
 */
void Population::clearPopulationList(PersonRole _role)
{
	for (size_t i = 0; i < mPopulationTable[_role].size(); ++i)
	{
		delete mPopulationTable[_role][i];
		mPopulationTable[_role][i] = nullptr;
	}
	mPopulationTable[_role].clear();
}


/**
 * Updates a specified segment of the population.
 * 
 * \param _role	Population segment to update.
 */
void Population::updatePersonList(PersonRole _role)
{
	for_each(mPopulationTable[_role].begin(), mPopulationTable[_role].end(), update_person);
}


/**
 * Deletes a specified Person from the list of female colonists.
 * 
 * \param	_p	Pointer to a Person.
 * 
 * \note	Checks if the given Person is male. If the Person is male, this function
 *			will fail.
 */
void Population::deleteFemale(Person* _p)
{
	if (_p->male())
		return;

	auto it = find(mPopulationFemale.begin(), mPopulationFemale.end(), _p);

	if (it == mPopulationFemale.end())
	{
		std::cout << "Population::deleteFemale(): Called on a non-female Person" << std::endl;
		return;
	}

	mPopulationFemale.erase(it);
}


/**
 * Deletes a given person from a specified Role.
 * 
 * \param	_p	Pointer to a Person.
 * \param	_r	Role of the Person specified in _p.
 */
void Population::deletePerson(Person* _p, PersonRole _r)
{
	deleteFemale(_p);

	auto it = find(mPopulationTable[_r].begin(), mPopulationTable[_r].end(), _p);
	if (it == mPopulationTable[_r].end())
	{
		std::cout << "Population::deletePerson(): Person object doesn't exist in the specified list." << std::endl;
		return;
	}

	mPopulationTable[_r].erase(it);
	delete _p;

	++mDeathCount;
}


/**
 * Populates a given population with a specified number of colonists given some base values.
 * 
 * \param	_role		Segment of the population to populate.
 * \param	_base_age	Base age in months of the population to populated.
 * \param	_age_jitter	Maximum difference from the Base Age of the colonist.
 * \param	_count		Number of colonists to create.
 */
void Population::populateList(PersonRole _role, int _base_age, int _age_jitter, unsigned int _count)
{
	for (unsigned int i = 0; i < _count; ++i)
	{
		Person* p = new Person(_base_age + (rand() % _age_jitter), i % 2 == 0);
		mPopulationTable[_role].push_back(p);

		if (!p->male())
		{
			p->birth_jitter(rand() % BIRTH_CHANCE_JITTER);
			mPopulationFemale.push_back(p);
		}
	}
}


/**
 * Gets the size of the entire population.
 */
int Population::size()
{
	int count = 0;
	for (auto it = mPopulationTable.begin(); it != mPopulationTable.end(); ++it)
		count += it->second.size();

	return count;
}


/**
 * Gets the size of a specific segment of the population.
 */
int Population::size(PersonRole _pr)
{
	return static_cast<int>(mPopulationTable[_pr].size());
}


/**
 * Gets the size of the female segment of the population.
 */
int Population::size_female()
{
	return mPopulationFemale.size();
}


/**
 * Iterates through all colonists of a given segment, determines if they live or die based on a given mortality
 * rate and moves them to a new segment of the population if they've reached the given transfer age.
 * 
 * \param	source_role			Role the Person is currently in.
 * \param	destination_role	Role the Person will be moved to if they've reached the transfer age.
 * \param	mortality			Mortality rate.
 * \param	transfer_age		Age, in weeks, at which the Person will be transferred to the destination role.
 */
void Population::checkRole(PersonRole source_role, PersonRole destination_role, int mortality, int transfer_age)
{
	for (size_t i = 0; i < mPopulationTable[source_role].size(); ++i)
	{

		if (rand() % DIVISOR <= mortality - morale_modifier(mCurrentMorale))
		{
			deletePerson(mPopulationTable[source_role][i], source_role);
			continue;
		}

		if (mPopulationTable[source_role][i]->age() >= transfer_age)
		{
			mPopulationTable[destination_role].push_back(mPopulationTable[source_role][i]);
			mPopulationTable[source_role].erase(mPopulationTable[source_role].begin() + i);
		}
	}
}


/**
 * Iterates through all students, determines if they live or die based on a given mortality
 * rate and transfers them to Worker or Scientist.
 */
void Population::check_students()
{
	for (size_t i = 0; i < mPopulationTable[ROLE_STUDENT].size(); ++i)
	{
		
		if (rand() % DIVISOR <= STUDENT_MORTALITY_RATE - morale_modifier(mCurrentMorale))
		{
			deletePerson(mPopulationTable[ROLE_STUDENT][i], ROLE_STUDENT);
			continue;
		}

		if (mPopulationTable[ROLE_STUDENT][i]->age() >= STUDENT_TO_ADULT)
		{
			if (rand() % 100 > STUDENT_TO_SCIENTIST_PERCENT)
				mPopulationTable[ROLE_WORKER].push_back(mPopulationTable[ROLE_STUDENT][i]);
			else
				mPopulationTable[ROLE_SCIENTIST].push_back(mPopulationTable[ROLE_STUDENT][i]);

			mPopulationTable[ROLE_STUDENT].erase(mPopulationTable[ROLE_STUDENT].begin() + i);
		}
	}
}


void Population::check_retired()
{
	for (size_t i = 0; i < mPopulationTable[ROLE_RETIRED].size(); ++i)
	{
		int modifier = 0;

		mPopulationTable[ROLE_RETIRED][i]->age() < AVERAGE_RETIRED_DEATH_AGE ? modifier = 0 : modifier = mPopulationTable[ROLE_RETIRED][i]->age() - AVERAGE_RETIRED_DEATH_AGE;

		if (rand() % DIVISOR <= ADULT_MORTALITY_RATE + modifier - morale_modifier(mCurrentMorale))
			deletePerson(mPopulationTable[ROLE_RETIRED][i], ROLE_RETIRED);
	}
}


/**
 * Iterates through list of female colonists and updates their conception status.
 */
void Population::check_females()
{
	for (size_t i = 0; i < mPopulationFemale.size(); ++i)
	{
		Person* _f = mPopulationFemale[i];
		if (_f->age() >= CHILD_BEARING_AGE_MIN && _f->age() <= CHILD_BEARING_AGE_MAX)
		{
			if (_f->pregnant() && _f->conceived() == 9)
			{
				++mBirthCount;

				mPopulationTable[ROLE_CHILD].push_back(new Person(0, (rand() % 100) % 2 != 0));

				if (!mPopulationTable[ROLE_CHILD].back()->male())
					mPopulationFemale.push_back(mPopulationTable[ROLE_CHILD].back());

				_f->birth_jitter(rand() % BIRTH_CHANCE_JITTER);
			}
			else if (!_f->pregnant())
			{
				_f->conceive();
			}
		}
	}
}


int Population::killPopulation(Population::PersonList& _pl, Population::PersonRole _pr, int count)
{
	int c = 0;
	for (size_t i = 0; i < _pl.size() && i < count; ++i)
	{
		deletePerson(_pl[0], _pr);
		++c;
	}

	return c;
}


/**
 * Determine how much food should be consumed and kill off any population that
 * starves.
 * 
 * \return	Actual amount of food consumed.
 */
int Population::consume_food(int _food)
{
	// If there's no food everybody dies.
	if (_food == 0)
	{
		clear();
		return 0;
	}

	int population_fed = _food * 10;
	if (population_fed > size())
	{
		return size() / 10;
	}

	/* Really not a good way to do this... whatever. Gets really slow with
	 * large population counts.
	 * 
	 * FIXME:	The float in the following equation is used to modify how many of the population starves.
	 *			It should change based on difficulty level.
	 * 
	 * NOTE:	Larger number means more of the population dies.
	 */
	int population_to_kill = (size() - population_fed) * (0.50f);

	if (size() == 1)
		population_to_kill = 1;

	for (int population_killed = 0; population_killed < population_to_kill;)
	{
		if (size() <= 0)
			break;

		PersonRole pr = static_cast<PersonRole>(rand() % ROLE_RETIRED + 1);
		PersonList& pl = mPopulationTable[pr];
		
		if (!pl.empty())
		{
			deletePerson(pl.back(), pr);
			++population_killed;
		}
	}

	return population_fed;
}


/**
 * \return	Actual amount of food consumed.
 */
int Population::update(int _morale, int _food)
{
	mBirthCount = 0;
	mDeathCount = 0;
	mCurrentMorale = _morale;

	updatePersonList(ROLE_CHILD);
	updatePersonList(ROLE_STUDENT);
	updatePersonList(ROLE_WORKER);
	updatePersonList(ROLE_SCIENTIST);
	updatePersonList(ROLE_RETIRED);

	checkRole(ROLE_CHILD, ROLE_STUDENT, CHILD_MORTALITY_RATE, CHILD_TO_STUDENT);
	checkRole(ROLE_WORKER, ROLE_RETIRED, ADULT_MORTALITY_RATE, WORKER_TO_RETIRE);
	checkRole(ROLE_SCIENTIST, ROLE_RETIRED, ADULT_MORTALITY_RATE, SCIENTIST_TO_RETIRE);

	check_students();
	check_females();
	check_retired();

	return consume_food(_food);
}
