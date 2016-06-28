#pragma once

#include "NAS2D/NAS2D.h"

class Mine
{
public:

	enum ProductionRate
	{
		PRODUCTION_RATE_LOW,
		PRODUCTION_RATE_MEDIUM,
		PRODUCTION_RATE_HIGH,
	};

	Mine(ProductionRate _rate);
	~Mine();

	bool active() const { return mActive; }
	void active(bool _b) { mActive = _b; }

	bool exhausted() const { return mExhausted; }

	bool yieldsCommonMetals() const { return commonMetalsRate() > 0; }
	bool yieldsRareMetals() const { return rareMetalsRate() > 0; }
	bool yieldsCommonMinerals() const { return commonMineralsRate() > 0; }
	bool yieldsRareMinerals() const { return rareMineralsRate() > 0; }

	ProductionRate productionRate() { return mProductionRate; }

	int age() const { return mAge; }

	int depth() const { return mDepth; }
	void depth(int _d) { mDepth = _d; }

	int commonMetalsRate() const { return mCommonMetalYield; }
	int rareMetalsRate() const { return mRareMetalYield; };
	int commonMineralsRate() const { return mCommonMineralYield; };
	int rareMineralsRate() const { return mRareMineralYield; };

	void update();

	void serialize(TiXmlElement* _ti);

private:
	Mine();

	void productionRate(ProductionRate _rate);

	void commonMetalsRate(int _rate) { mCommonMetalYield = _rate; }
	void rareMetalsRate(int _rate) { mRareMetalYield = _rate; };
	void commonMineralsRate(int _rate) { mCommonMineralYield = _rate; };
	void rareMineralsRate(int _rate) { mRareMineralYield = _rate; };

private:
	int						mAge;						/**< Age of the mine in turns. */
	int						mDepth;						/**< Depth of the Mine. */

	int						mCommonMetalYield;			/**<  */
	int						mRareMetalYield;			/**<  */
	int						mCommonMineralYield;		/**<  */
	int						mRareMineralYield;			/**<  */

	ProductionRate			mProductionRate;			/**< Mine's production rate. */

	bool					mActive;					/**< Flag indicating whether this mine is active or not. */
	bool					mExhausted;					/**< Flag indicating that this mine can no longer produce anything. */
};
