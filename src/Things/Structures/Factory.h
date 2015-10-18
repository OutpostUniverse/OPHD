#pragma once

#include "Structure.h"

#include "../../RobotPool.h"

class Factory : public Structure
{

public:

	Factory(const std::string& name, const std::string& sprite_path);

	virtual ~Factory();

	virtual void updateProduction();

	void resourcePool(Resources* _r) { mResourcesPool = _r; }
	void robotPool(RobotPool* _r) { mRobotPool = _r; }

protected:
	virtual void initFactory() {}

	virtual void productionComplete() {}

	int productionLength() const { return mProductionLength; }
	void productionLength(int _l) { mProductionLength = _l; }

	int productionTime() const { return mProductionTime; }

	void resetProductionTime() { mProductionTime = 0; }

	Resources* resourcePool() { return mResourcesPool; }
	RobotPool* robotPool() { return mRobotPool; }

private:

	int								mProductionTime;
	int								mProductionLength;

	Resources						mProductionInput;

	Resources*						mResourcesPool;					/**< Pointer to the player's resource pool. UGLY. */
	RobotPool*						mRobotPool;						/**< Pointer to the player's robot pool. UGLY. */
};