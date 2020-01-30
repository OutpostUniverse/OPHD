// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Mine.h"

#include <iostream>

using namespace NAS2D::Xml;


/**
 * Helper function that gets the total amount of ore 
 */
static int getOreCount(const Mine::MineVeins& veins, Mine::OreType ore, size_t depth)
{
	int _value = 0;
	for (size_t i = 0; i < depth; ++i)
	{
		_value += veins[i][ore];
	}
	return _value;
}


static void setDefaultFlags(std::bitset<6>& flags)
{
	flags[Mine::ORE_COMMON_METALS] = true;
	flags[Mine::ORE_COMMON_MINERALS] = true;
	flags[Mine::ORE_RARE_METALS] = true;
	flags[Mine::ORE_RARE_MINERALS] = true;
	flags[4] = false;
	flags[5] = false;
}


/**
 * C'tor
 */
Mine::Mine()
{
	setDefaultFlags(mFlags);
}


/**
 * C'tor
 */
Mine::Mine(MineProductionRate rate) : mProductionRate(rate)
{
	setDefaultFlags(mFlags);
}


/**
 * 
 */
bool Mine::active() const
{
	return mFlags[4];
}


/**
 * 
 */
void Mine::active(bool _b)
{
	mFlags[4] = _b;
}


/**
 * 
 */
bool Mine::miningCommonMetals() const
{
	return mFlags[ORE_COMMON_METALS];
}


/**
 * 
 */
bool Mine::miningCommonMinerals() const
{
	return mFlags[ORE_COMMON_MINERALS];
}


/**
 * 
 */
bool Mine::miningRareMetals() const
{
	return mFlags[ORE_RARE_METALS];
}


/**
 * 
 */
bool Mine::miningRareMinerals() const
{
	return mFlags[ORE_RARE_MINERALS];
}


/**
 * 
 */
void Mine::miningCommonMetals(bool _b)
{
	mFlags[ORE_COMMON_METALS] = _b;
}


/**
 * 
 */
void Mine::miningCommonMinerals(bool _b)
{
	mFlags[ORE_COMMON_MINERALS] = _b;
}


/**
 * 
 */
void Mine::miningRareMetals(bool _b)
{
	mFlags[ORE_RARE_METALS] = _b;
}


/**
 * 
 */
void Mine::miningRareMinerals(bool _b)
{
	mFlags[ORE_RARE_MINERALS] = _b;
}


/**
 * Increases the depth of the mine.
 * 
 * \note	This function only modifies the Mine. It has no knowledge of the
 *			maximum digging depth of a planet and doesn't modify any tiles.
 */
void Mine::increaseDepth()
{
	MineVein vein;

	switch (productionRate())
	{
	case PRODUCTION_RATE_LOW:
		vein[ORE_COMMON_METALS] = 600;
		vein[ORE_COMMON_MINERALS] = 500;
		vein[ORE_RARE_METALS] = 600;
		vein[ORE_RARE_MINERALS] = 500;
		break;

	case PRODUCTION_RATE_MEDIUM:
		vein[ORE_COMMON_METALS] = 700;
		vein[ORE_COMMON_MINERALS] = 550;
		vein[ORE_RARE_METALS] = 700;
		vein[ORE_RARE_MINERALS] = 550;
		break;

	case PRODUCTION_RATE_HIGH:
		vein[ORE_COMMON_METALS] = 850;
		vein[ORE_COMMON_MINERALS] = 600;
		vein[ORE_RARE_METALS] = 850;
		vein[ORE_RARE_MINERALS] = 600;
		break;
	}

	mVeins.push_back(vein);
}


/**
 * Gets the current depth of the mine.
 */
int Mine::depth() const
{
	return mVeins.size();
}


/**
 * Convenience function that gets the common metal ore available
 * given the current level.
 */
int Mine::commonMetalsAvailable() const
{
	return getOreCount(mVeins, ORE_COMMON_METALS, depth());
}


/**
 * Convenience function that gets the common mineral ore available
 * given the current level.
 */
int Mine::commonMineralsAvailable() const
{
	return getOreCount(mVeins, ORE_COMMON_MINERALS, depth());
}


/**
 * Convenience function that gets the rare metal ore available
 * given the current level.
 */
int Mine::rareMetalsAvailable() const
{
	return getOreCount(mVeins, ORE_RARE_METALS, depth());
}


/**
 * Convenience function that gets the rare mineral ore available
 * given the current level.
 */
int Mine::rareMineralsAvailable() const
{
	return getOreCount(mVeins, ORE_RARE_MINERALS, depth());
}


/**
 * Indicates that there are no resources available at this mine.
 */
bool Mine::exhausted() const
{
	return mFlags[5];
}


/**
 * Checks if the mine is exhausted and if it is sets the exhausted flag.
 * 
 * \note	This function is provided so that this computation is only
 *			done once per turn instead of being done every frame. The
 *			issue came up after updating the minimap code to indicate
 *			exhausted mines.
 */
void Mine::checkExhausted()
{
	if (!active()) { return; }
	
	int ore_count = 0;
	for (auto vein : mVeins)
	{
		ore_count += vein[ORE_COMMON_METALS];
		ore_count += vein[ORE_COMMON_MINERALS];
		ore_count += vein[ORE_RARE_METALS];
		ore_count += vein[ORE_RARE_MINERALS];
	}

	mFlags[5] = (ore_count == 0);
}


/**
 * Pulls the specified quantity of Ore from the Mine. If
 * insufficient ore is available, only pulls what's available.
 */
int Mine::pull(OreType type, int quantity)
{
	int pulled_count = 0, to_pull = quantity;

	for (size_t i = 0; i < mVeins.size(); ++i)
	{
		MineVein& vein = mVeins[i];

		if (vein[type] >= to_pull)
		{
			pulled_count = to_pull;
			vein[type] -= to_pull;
			break;
		}
		else if (vein[type] < to_pull)
		{
			pulled_count += vein[type];
			to_pull = to_pull - vein[type];
			vein[type] = 0;
		}
	}

	return pulled_count;
}


// ===============================================================================


/**
 * Serializes current mine information.
 */
void Mine::serialize(XmlElement* _ti)
{
	_ti->attribute("depth", depth());
	_ti->attribute("active", active());
	_ti->attribute("yield", productionRate());
	_ti->attribute("flags", mFlags.to_string());

	for (size_t i = 0; i < mVeins.size(); ++i)
	{
		const MineVein& mv = mVeins[i];

		XmlElement* vein = new XmlElement("vein");

		vein->attribute("id", static_cast<int>(i));
		vein->attribute("common_metals",	mv[ORE_COMMON_METALS]);
		vein->attribute("common_minerals",	mv[ORE_COMMON_MINERALS]);
		vein->attribute("rare_metals",		mv[ORE_RARE_METALS]);
		vein->attribute("rare_minerals",	mv[ORE_RARE_MINERALS]);

		_ti->linkEndChild(vein);
	}
}


/**
 * 
 */
void Mine::deserialize(NAS2D::Xml::XmlElement* _ti)
{
	int active = 0, yield = 0, depth = 0;
	std::string flags;

	XmlAttribute* attribute = _ti->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == "active") { attribute->queryIntValue(active); }
		else if (attribute->name() == "depth") { attribute->queryIntValue(depth); }
		else if (attribute->name() == "yield") { attribute->queryIntValue(yield); }
		else if (attribute->name() == "flags") { mFlags = std::bitset<6>(attribute->value()); }
		attribute = attribute->next();
	}

	this->active(active != 0);
	mProductionRate = static_cast<MineProductionRate>(yield);

	mVeins.resize(depth);
	for (XmlNode* vein = _ti->firstChild(); vein != nullptr; vein = vein->nextSibling())
	{
		MineVein _mv = MineVein{0, 0, 0, 0};
		attribute = vein->toElement()->firstAttribute();
		int id = 0;
		while (attribute)
		{
			if (attribute->name() == "common_metals") { attribute->queryIntValue(_mv[ORE_COMMON_METALS]); }
			else if (attribute->name() == "common_minerals") { attribute->queryIntValue(_mv[ORE_COMMON_MINERALS]); }
			else if (attribute->name() == "rare_metals") { attribute->queryIntValue(_mv[ORE_RARE_METALS]); }
			else if (attribute->name() == "rare_minerals") { attribute->queryIntValue(_mv[ORE_RARE_MINERALS]); }
			else if (attribute->name() == "id") { attribute->queryIntValue(id); }
			attribute = attribute->next();
		}
		mVeins[id] = _mv;
	}
}
