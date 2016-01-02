#pragma once

/**
* Pretty much just an easy container for keeping track of resources.
* 
* \note	Plain Jane structure. No sanity checking at all.
*/
class ResourcePool
{
public:
	ResourcePool();

	ResourcePool(const ResourcePool& _r);
	
	~ResourcePool();

	ResourcePool& operator=(const ResourcePool& rhs);
	ResourcePool& operator+=(const ResourcePool& rhs);
	ResourcePool& operator-=(const ResourcePool& rhs);

	friend bool operator<(const ResourcePool& lhs, const ResourcePool& rhs);
	friend bool operator>(const ResourcePool& lhs, const ResourcePool& rhs);
	friend bool operator<=(const ResourcePool& lhs, const ResourcePool& rhs);
	friend bool operator>=(const ResourcePool& lhs, const ResourcePool& rhs);

	/**
	* Sets all values to 0.
	*/
	void clear();

	// GETTERS
	double commonMetalsOre() const { return _commonMetalsOre; }
	double commonMineralsOre() const { return _commonMineralsOre; }
	double rareMetalsOre() const { return _rareMetalsOre; }
	double rareMineralsOre() const { return _rareMineralsOre; }

	double commonMetals() const { return _commonMetals; }
	double commonMinerals() const { return _commonMinerals; }
	double rareMetals() const { return _rareMetals; }
	double rareMinerals() const { return _rareMinerals; }

	int energy() const { return _energy; }
	int food() const { return _food; }

	// SETTERS
	void commonMetalsOre(double _d) { _commonMetalsOre = _d; }
	void commonMineralsOre(double _d) { _commonMineralsOre = _d; }
	void rareMetalsOre(double _d) { _rareMetalsOre = _d; }
	void rareMineralsOre(double _d) { _rareMineralsOre = _d; }

	void commonMetals(double _d) { _commonMetals = _d; }
	void commonMinerals(double _d) { _commonMinerals = _d; }
	void rareMetals(double _d) { _rareMetals = _d; }
	void rareMinerals(double _d) { _rareMinerals = _d; }

	void energy(int _i)  { _energy = _i; }
	void food(int _i)  { _food = _i; }

protected:

private:

	// Ore
	double _commonMetalsOre;
	double _commonMineralsOre;
	double _rareMetalsOre;
	double _rareMineralsOre;

	// Refined Materials
	double _commonMetals;
	double _commonMinerals;
	double _rareMetals;
	double _rareMinerals;

	// Other
	int	_energy;
	int	_food;
};
