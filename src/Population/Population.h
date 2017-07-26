#pragma once

#include "Person.h"

#include <map>
#include <vector>

class Population
{
public:
	typedef std::vector<Person*> PersonList;

	enum PersonRole
	{
		ROLE_CHILD,
		ROLE_STUDENT,
		ROLE_WORKER,
		ROLE_SCIENTIST,
		ROLE_RETIRED
	};

public:
	Population();
	~Population();

	int size();
	int size(PersonRole);
	int size_female();

	int birthCount() const { return mBirthCount; }
	int deathCount() const { return mDeathCount; }

	void clear();

	void populateList(PersonRole _role, int _base_age, int _age_jitter, unsigned int _count);

	int update(int _morale, int _food);

protected:

private:
	void clearPopulationList(PersonRole);

	void updatePersonList(PersonRole);

	void deleteFemale(Person* _p);
	void deletePerson(Person* _p, PersonRole _r);

	void checkRole(PersonRole source_role, PersonRole destination_role, int mortality, int trasnfer_age);

	void check_students();
	void check_females();
	void check_retired();

	int consume_food(int _food);

	int killPopulation(Population::PersonList& _pl, Population::PersonRole _pr, size_t count);

private:
	typedef std::map<PersonRole, PersonList> PopulationTable;

private:

	int					mBirthCount;
	int					mDeathCount;
	int					mCurrentMorale;

	PopulationTable		mPopulationTable;

	PersonList			mPopulationFemale;
};
