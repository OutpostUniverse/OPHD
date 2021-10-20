#pragma once

/**
 * Defines cost in materials per turn.
 *
 * Basically just a storage class used to contain resource costs per turn and turn count
 * needed to produce a particular item.
 */
class ProductionCost
{
public:
	ProductionCost() = default;

	ProductionCost(int turns, int commonMetals, int commonMinerals, int rareMetals, int rareMinerals) :
		mTurnsToBuild(turns),
		mCommonMetals(commonMetals),
		mCommonMinerals(commonMinerals),
		mRareMetals(rareMetals),
		mRareMinerals(rareMinerals)
	{}

	int turnsToBuild() const { return mTurnsToBuild; }
	int commonMetals() const { return mCommonMetals; }
	int commonMinerals() const { return mCommonMinerals; }
	int rareMetals() const { return mRareMetals; }
	int rareMinerals() const { return mRareMinerals; }

private:
	int mTurnsToBuild = 0;
	int mCommonMetals = 0;
	int mCommonMinerals = 0;
	int mRareMetals = 0;
	int mRareMinerals = 0;
};
