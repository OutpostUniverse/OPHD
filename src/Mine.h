#ifndef __MINE__
#define __MINE__


class Mine
{
public:

	enum ProductionRate
	{
		PRODUCTION_RATE_LOW,
		PRODUCTION_RATE_MEDIUM,
		PRODUCTION_RATE_HIGH,
	};

	Mine();
	Mine(ProductionRate _rate);
	~Mine();

	bool active() const { return mActive; }
	void active(bool _b) { mActive = _b; }

	bool yieldsCommonMetals() const { return mYieldsCommonMetals; }
	void yieldsCommonMetals(bool _b) { mYieldsCommonMetals = _b; }

	bool yieldsRareMetals() const { return mYieldsRareMetals; }
	void yieldsRareMetals(bool _b) { mYieldsRareMetals = _b; }

	bool yieldsCommonMinerals() const { return mYieldsCommonMinerals; }
	void yieldsCommonMinerals(bool _b) { mYieldsCommonMinerals = _b; }

	bool yieldsRareMinerals() const { return mYieldsRareMinerals; }
	void yieldsRareMinerals(bool _b) { mYieldsRareMinerals = _b; }

	ProductionRate productionRate() { return mProductionRate; }
	void productionRate(ProductionRate _rate);

	int age() const { return mAge; }

	void update();

	void commonMetalsRate(int _rate) { mCommonMetalYieldRate = _rate; }
	void rareMetalsRate(int _rate) { mRareMetalYieldRate = _rate; };
	void commonMineralsRate(int _rate) { mCommonMineralYieldRate = _rate; };
	void rareMineralsRate(int _rate) { mRareMineralYieldRate = _rate; };

	int commonMetalsRate() const { return mCommonMetalYieldRate; }
	int rareMetalsRate() const { return mRareMetalYieldRate; };
	int commonMineralsRate() const { return mCommonMineralYieldRate; };
	int rareMineralsRate() const { return mRareMineralYieldRate; };

private:

	int						mAge;							/**< Age of the mine in turns. */
	int						mDepth;							/**< Depth of the Mine. */

	int						mCommonMetalYieldRate;			/**<  */
	int						mRareMetalYieldRate;			/**<  */
	int						mCommonMineralYieldRate;		/**<  */
	int						mRareMineralYieldRate;			/**<  */

	ProductionRate			mProductionRate;				/**< Mine's production rate. */

	bool					mYieldsCommonMetals;			/**< The Mine will produce Common Metals Ore. */
	bool					mYieldsRareMetals;				/**< The Mine will produce Rare Metals Ore. */
	bool					mYieldsCommonMinerals;			/**< The Mine will produce Common Minerals Ore. */
	bool					mYieldsRareMinerals;			/**< The Mine will produce Rare Mineral Ore. */

	bool					mActive;						/**< Flag indicating whether this mine is active or not. */
	bool					mExhausted;						/**< Flag indicating that this mine can no longer produce anything. */
};


#endif