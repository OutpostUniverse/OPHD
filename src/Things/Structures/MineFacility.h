#pragma once

#include "Structure.h"

#include "../../Mine.h"

/**
 * Implements the Mine Facility.
 */
class MineFacility: public Structure
{
public:
	MineFacility(Mine* mine);
	virtual ~MineFacility();
	
	void mine(Mine* _m) { mMine = _m; }

protected:
	virtual void think();

protected:
	friend class MineOperationsWindow;

	Mine* mine() { return mMine; }

private:
	MineFacility() = delete;
	MineFacility(const MineFacility&) = delete;
	MineFacility& operator=(const MineFacility&) = delete;

private:
	virtual void defineResourceInput() { mMine->active(true); }
	virtual void defineResourceOutput()	{}

	virtual void activated() final;

private:
	Mine*			mMine;
};
