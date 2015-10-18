#ifndef __RESOURCES__
#define __RESOURCES__

/**
 * Pretty much just an easy container for keeping track of resources.
 * 
 * \note	Plain Jane structure. No sanity checking at all.
 */
struct Resources
{
public:
	Resources():	commonMetalsOre(0),
					commonMineralsOre(0),
					rareMetalsOre(0),
					rareMineralsOre(0),
					commonMetals(0),
					commonMinerals(0),
					rareMetals(0),
					rareMinerals(0),
					energy(0),
					food(0)
	{}

	Resources(const Resources& _r):	commonMetalsOre(_r.commonMetalsOre),
									commonMineralsOre(_r.commonMineralsOre),
									rareMetalsOre(_r.rareMetalsOre),
									rareMineralsOre(_r.rareMineralsOre),
									commonMetals(_r.commonMetals),
									commonMinerals(_r.commonMinerals),
									rareMetals(_r.rareMetals),
									rareMinerals(_r.rareMinerals),
									energy(_r.energy),
									food(_r.food)
	{}
	
	~Resources() {}

	 Resources& operator=(const Resources& rhs)
	 {
		 commonMetalsOre = rhs.commonMetalsOre;
		 commonMineralsOre = rhs.commonMineralsOre;
		 rareMetalsOre = rhs.rareMetalsOre;
		 rareMineralsOre = rhs.rareMineralsOre;

		 commonMetals = rhs.commonMetals;
		 commonMinerals = rhs.commonMinerals;
		 rareMetals = rhs.rareMetals;
		 rareMinerals = rhs.rareMinerals;

		 energy = rhs.energy;
		 food = rhs.food;

		 return *this;
	 }

	 Resources& operator+=(const Resources& rhs)
	 {
		 commonMetalsOre += rhs.commonMetalsOre;
		 commonMineralsOre += rhs.commonMineralsOre;
		 rareMetalsOre += rhs.rareMetalsOre;
		 rareMineralsOre += rhs.rareMineralsOre;

		 commonMetals += rhs.commonMetals;
		 commonMinerals += rhs.commonMinerals;
		 rareMetals += rhs.rareMetals;
		 rareMinerals += rhs.rareMinerals;

		 energy += rhs.energy;
		 food += rhs.food;

		 return *this;
	 }

 	 Resources& operator-=(const Resources& rhs)
	 {
		 commonMetalsOre -= rhs.commonMetalsOre;
		 commonMineralsOre -= rhs.commonMineralsOre;
		 rareMetalsOre -= rhs.rareMetalsOre;
		 rareMineralsOre -= rhs.rareMineralsOre;

		 commonMetals -= rhs.commonMetals;
		 commonMinerals -= rhs.commonMinerals;
		 rareMetals -= rhs.rareMetals;
		 rareMinerals -= rhs.rareMinerals;

		 energy -= rhs.energy;
		 food -= rhs.food;

		 return *this;
	 }

	// Ore
	double commonMetalsOre;
	double commonMineralsOre;
	double rareMetalsOre;
	double rareMineralsOre;

	// Refined Materials
	double commonMetals;
	double commonMinerals;
	double rareMetals;
	double rareMinerals;

	// Other
	int	energy;
	int	food;
};

#endif