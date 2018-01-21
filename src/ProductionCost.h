#pragma once

/**
 * \brief	Defines cost in materials per turn.
 * 
 * Basically just a storage class used to contain resource costs per turn and turn count
 * needed to produce a particular item.
 */
class ProductionCost
{
public:
	ProductionCost() {}

	ProductionCost(int turns, int commonMetals, int commonMinerals, int rareMetals, int rareMinerals) :
		mTurnsToBuild(turns),
		mCommonMetals(commonMetals),
		mCommonMinerals(commonMinerals),
		mRareMetals(rareMetals),
		mRareMinerals(rareMinerals)
	{}

	~ProductionCost() {}

	void clear()
	{
		mCommonMetals = 0;
		mCommonMinerals = 0;
		mRareMetals = 0;
		mRareMinerals = 0;
		mTurnsToBuild = 0;
	}

	int commonMetals() const { return mCommonMetals; }
	int commonMinerals() const { return mCommonMinerals; }
	int rareMetals() const { return mRareMetals; }
	int rareMinerals() const { return mRareMinerals; }
	int turnsToBuild() const { return mTurnsToBuild; }

private:
	int				mCommonMetals = 0;
	int				mCommonMinerals = 0;
	int				mRareMetals = 0;
	int 			mRareMinerals = 0;
	int				mTurnsToBuild = 0;
};
