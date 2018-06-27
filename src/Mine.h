#pragma once

#include "Common.h"
#include "Constants.h"

#include "NAS2D/NAS2D.h"

/**
 * 
 */
class Mine
{
public:
	enum OreType
	{
		ORE_COMMON_METALS,
		ORE_COMMON_MINERALS,
		ORE_RARE_METALS,
		ORE_RARE_MINERALS,
	};

	typedef std::array<int, 4> MineVein;
	typedef std::vector<MineVein> MineVeins;	/**<  */

public:
	Mine() {}
	Mine(MineProductionRate rate) : mProductionRate(rate) {};
	~Mine() {};

	bool active() const { return mActive; }
	void active(bool _b) { mActive = _b; }

	bool exhausted() const;

	MineProductionRate productionRate() { return mProductionRate; }

	int depth() const;
	void increaseDepth();

	int commonMetalsAvailable() const;
	int commonMineralsAvailable() const;
	int rareMetalsAvailable() const;
	int rareMineralsAvailable() const;

	void serialize(NAS2D::Xml::XmlElement* _ti);
	void deserialize(NAS2D::Xml::XmlElement* _ti);

private:
	Mine(const Mine&) = delete;
	Mine& operator=(const Mine&) = delete;

private:
	MineVeins			mVeins;									/**< Ore veins */
	MineProductionRate	mProductionRate = PRODUCTION_RATE_LOW;	/**< Mine's production rate. */
	bool				mActive = false;						/**< Flag indicating whether this mine is active or not. */
};
