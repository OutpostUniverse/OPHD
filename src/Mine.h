#pragma once

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

	bool yieldsCommonMetals() const { return commonMetalsRate() > 0; }
	bool yieldsRareMetals() const { return rareMetalsRate() > 0; }
	bool yieldsCommonMinerals() const { return commonMineralsRate() > 0; }
	bool yieldsRareMinerals() const { return rareMineralsRate() > 0; }

	ProductionRate productionRate() { return mProductionRate; }

	int age() const { return mAge; }

	int depth() const { return mDepth; }
	void depth(int _d) { mDepth = _d; }

	int commonMetalsRate() const { return mCommonMetalYieldRate; }
	int rareMetalsRate() const { return mRareMetalYieldRate; };
	int commonMineralsRate() const { return mCommonMineralYieldRate; };
	int rareMineralsRate() const { return mRareMineralYieldRate; };

	void update();

private:
	Mine();

	void productionRate(ProductionRate _rate);

	void commonMetalsRate(int _rate) { mCommonMetalYieldRate = _rate; }
	void rareMetalsRate(int _rate) { mRareMetalYieldRate = _rate; };
	void commonMineralsRate(int _rate) { mCommonMineralYieldRate = _rate; };
	void rareMineralsRate(int _rate) { mRareMineralYieldRate = _rate; };

private:
	int						mAge;							/**< Age of the mine in turns. */
	int						mDepth;							/**< Depth of the Mine. */

	int						mCommonMetalYieldRate;			/**<  */
	int						mRareMetalYieldRate;			/**<  */
	int						mCommonMineralYieldRate;		/**<  */
	int						mRareMineralYieldRate;			/**<  */

	ProductionRate			mProductionRate;				/**< Mine's production rate. */

	bool					mActive;						/**< Flag indicating whether this mine is active or not. */
	bool					mExhausted;						/**< Flag indicating that this mine can no longer produce anything. */
};
