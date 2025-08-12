#include "Structure.h"

#include "StructureState.h"
#include "StructureClass.h"
#include "StructureIdToClass.h"

#include "../StructureCatalog.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../Map/Tile.h"

#include "../UI/StringTable.h"

#include <libOPHD/EnumConnectorDir.h>
#include <libOPHD/EnumDisabledReason.h>
#include <libOPHD/EnumIdleReason.h>
#include <libOPHD/MapObjects/StructureType.h>
#include <libOPHD/RandomNumberGenerator.h>
#include <libOPHD/MeanSolarDistance.h>

#include <NAS2D/Dictionary.h>

#include <algorithm>


const std::map<StructureState, std::string> StructureStateDescriptions =
{
	{StructureState::UnderConstruction, "Under Construction"},
	{StructureState::Operational, "Operational"},
	{StructureState::Idle, "Idle"},
	{StructureState::Disabled, "Disabled"},
	{StructureState::Destroyed, "Destroyed"},
};


Structure::Structure(StructureID id, Tile& tile) :
	Structure{id, tile, constants::StructureStateConstruction}
{
}


Structure::Structure(StructureID id, Tile& tile, const std::string& initialAction) :
	MapObject{StructureCatalog::getType(id).spritePath, initialAction},
	mStructureType{StructureCatalog::getType(id)},
	mStructureId{id},
	mStructureClass{structureIdToClass(id)},
	mTile{tile},
	mStructureState{StructureState::UnderConstruction},
	mConnectorDirection{ConnectorDir::Intersection},
	mDisabledReason{DisabledReason::None},
	mIdleReason{IdleReason::None}
{
}


const StructureType& Structure::type() const
{
	return mStructureType;
}


const std::string& Structure::name() const
{
	return mStructureType.name;
}


Tile& Structure::tile() const
{
	return mTile;
}


const MapCoordinate& Structure::xyz() const
{
	return mTile.xyz();
}


bool Structure::disabled() const
{
	return mStructureState == StructureState::Disabled;
}


/**
 * Sets a Disabled state for the Structure.
 */
void Structure::disable(DisabledReason reason)
{
	mSprite.pause();
	mSprite.color(NAS2D::Color{255, 0, 0, 185});
	mStructureState = StructureState::Disabled;
	mDisabledReason = reason;
	mIdleReason = IdleReason::None;
	disabledStateSet();
}


bool Structure::operational() const
{
	return mStructureState == StructureState::Operational;
}


/**
* Sets an Operational state for the Structure.
*/
void Structure::enable()
{
	if (forceIdle())
	{
		idle(IdleReason::PlayerSet);
		return;
	}

	mSprite.resume();
	mSprite.color(NAS2D::Color::White);
	mStructureState = StructureState::Operational;
	mDisabledReason = DisabledReason::None;
	mIdleReason = IdleReason::None;
}


bool Structure::isIdle() const
{
	return mStructureState == StructureState::Idle;
}


/**
* Sets idle state of the Structure.
*/
void Structure::idle(IdleReason reason)
{
	if (forceIdle() && !disabled())
	{
		return;
	}

	mSprite.pause();
	mSprite.color(NAS2D::Color{255, 255, 255, 185});
	mDisabledReason = DisabledReason::None;
	mIdleReason = reason;
	mStructureState = StructureState::Idle;
}


bool Structure::destroyed() const
{
	return mStructureState == StructureState::Destroyed;
}


bool Structure::underConstruction() const
{
	return mStructureState == StructureState::UnderConstruction;
}


/**
 * Forces an idle state. Used to prevent automatic enabling of the
 * Structure.
 */
void Structure::forceIdle(bool forcedIdle)
{
	if (disabled() || destroyed()) { return; }

	// Note that the order in which the flag is set matters
	// in terms of the logic involved here.
	if (forcedIdle)
	{
		idle(IdleReason::PlayerSet);
		mForcedIdle = true;
	}
	else
	{
		mForcedIdle = false;
		enable();
	}
}

const StorableResources& Structure::resourcesIn() const
{
	return mStructureType.operationalCost;
}

const PopulationRequirements& Structure::populationRequirements() const
{
	return mStructureType.populationRequirements;
}

StructureClass Structure::structureClass() const
{
	return mStructureClass;
}

const std::string& Structure::stateDescription() const
{
	return stateDescription(state());
}

const std::string& Structure::stateDescription(StructureState state)
{
	return StructureStateDescriptions.at(state);
}

const std::string& Structure::classDescription() const
{
	return mStructureType.description;
}

int Structure::turnsToBuild() const
{
	return mStructureType.turnsToBuild;
}

int Structure::maxAge() const
{
	return mStructureType.maxAge;
}

int Structure::energyRequirement() const
{
	return mStructureType.energyRequired;
}

int Structure::energyProduced() const
{
	return operational() ? calculateMaxEnergyProduction() : 0;
}

int Structure::calculateMaxEnergyProduction() const
{
	return mStructureType.energyProduced + scaleSolarOutput(mStructureType.solarEnergyProduced);
}

int Structure::foodProduced() const
{
	return operational() ? mStructureType.foodProduced : 0;
}

int Structure::foodStorageCapacity() const
{
	return mStructureType.foodStorageCapacity;
}

int Structure::refinedOreStorageCapacity() const
{
	return mStructureType.oreStorageCapacity;
}

int Structure::commRange() const
{
	return operational() ? mStructureType.commRange : 0;
}

int Structure::policeRange() const
{
	return operational() ? mStructureType.policeRange : 0;
}

bool Structure::hasCrime() const
{
	return mStructureType.isCrimeTarget;
}

int Structure::integrityDecayRate() const
{
	return mStructureType.integrityDecayRate;
}

bool Structure::requiresCHAP() const
{
	return mStructureType.isChapRequired;
}

bool Structure::selfSustained() const
{
	return mStructureType.isSelfSustained;
}

bool Structure::repairable() const
{
	return mStructureType.isRepairable && (mStructureState != StructureState::Destroyed);
}

bool Structure::isChap() const { return mStructureClass == StructureClass::LifeSupport; }
bool Structure::isCommand() const { return mStructureClass == StructureClass::Command; }
bool Structure::isFactory() const { return mStructureClass == StructureClass::Factory; }
bool Structure::isWarehouse() const { return mStructureClass == StructureClass::Warehouse; }
bool Structure::isRobotCommand() const { return mStructureClass == StructureClass::RobotCommand; }
bool Structure::isMineFacility() const { return mStructureClass == StructureClass::Mine; }
bool Structure::isSmelter() const { return mStructureClass == StructureClass::Smelter; }
bool Structure::isEnergyProducer() const { return mStructureType.energyProduced > 0 || mStructureType.solarEnergyProduced > 0; }
bool Structure::isFoodProducer() const { return mStructureType.foodProduced > 0; }
bool Structure::isFoodStore() const { return mStructureType.foodStorageCapacity > 0; }
bool Structure::isOreStore() const { return mStructureType.oreStorageCapacity > 0; }
bool Structure::isComms() const { return mStructureType.commRange > 0; }
bool Structure::isPolice() const { return mStructureType.policeRange > 0; }
bool Structure::isLander() const { return mStructureClass == StructureClass::Lander; }
bool Structure::isPark() const { return mStructureClass == StructureClass::Park; }
bool Structure::isMaintenance() const { return mStructureClass == StructureClass::Maintenance; }
bool Structure::isConnector() const { return mStructureClass == StructureClass::Tube; }
bool Structure::isRoad() const { return mStructureClass == StructureClass::Road; }

/**
 * Called when a building is finished being built.
 *
 * Sets the animation state of the Structure to Operational,
 * sets the building state to Operational and sets resource
 * requirements.
 */
void Structure::activate()
{
	mSprite.play(constants::StructureStateOperational);
	enable();

	activated();
}


void Structure::rebuild()
{
	mSprite.play(constants::StructureStateConstruction);
	mStructureState = StructureState::UnderConstruction;

	age(1);
}


void Structure::processTurn()
{
	if (destroyed()) { return; }
	incrementAge();
	updateIntegrityDecay();
}


/**
 * Updates age of the structure and performs some basic age management logic.
 */
void Structure::incrementAge()
{
	mAge++;

	if (age() == turnsToBuild())
	{
		activate();
	}
	else if (maxAge() == 0)
	{
		/**
		 * Structures defined with a max age of '0' are 'ageless'
		 * and should continue to operate regardless of how old
		 * they are (for example, the Command Center).
		 */
	}
	else if (age() == maxAge())
	{
		destroy();
	}
}


void Structure::updateIntegrityDecay()
{
	// structures being built don't decay
	if (state() == StructureState::UnderConstruction) { return; }

	mIntegrity = std::clamp(mIntegrity - integrityDecayRate(), 0, mIntegrity);

	if (mIntegrity <= 35 && !disabled())
	{
		disable(DisabledReason::StructuralIntegrity);
	}
	else if (mIntegrity <= 20 && !destroyed())
	{
		/* range is 0 - 1000, 0 - 100 for 10% chance */
		if (randomNumber.generate(0, 1000) < 100)
		{
			destroy();
		}
	}
	else if (mIntegrity <= 0)
	{
		mIntegrity = 0;
		destroy();
	}
}


/**
* Sets a destroyed state.
*
* \note		This is permanent.
*/
void Structure::destroy()
{
	mSprite.play(constants::StructureStateDestroyed);
	mStructureState = StructureState::Destroyed;
}


/**
 * Provided for loading purposes.
 */
void Structure::forcedStateChange(StructureState structureState, DisabledReason disabledReason, IdleReason idleReason)
{
	if (age() >= turnsToBuild())
	{
		mSprite.play(constants::StructureStateOperational);
		//enable();
	}

	if (structureState == StructureState::Operational) { enable(); }
	else if (structureState == StructureState::Idle) { idle(idleReason); }
	else if (structureState == StructureState::Disabled) { disable(disabledReason); }
	else if (structureState == StructureState::Destroyed) { destroy(); }
	else if (structureState == StructureState::UnderConstruction) { mStructureState = StructureState::UnderConstruction; } // Kludge
}


void Structure::crimeRate(int crimeRate)
{
	mCrimeRate = std::clamp(crimeRate, 0, 100);
}


void Structure::increaseCrimeRate(int deltaCrimeRate)
{
	mCrimeRate = std::clamp(mCrimeRate + deltaCrimeRate, 0, 100);
}


void Structure::integrity(int integrity)
{
	mIntegrity = integrity;
}


StringTable Structure::createInspectorViewTable() const
{
	if (calculateMaxEnergyProduction() > 0)
	{
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Power Produced:";

		auto produced = energyProduced();

		stringTable[{1, 0}].text = std::to_string(produced) + " / " + std::to_string(calculateMaxEnergyProduction());

		if (produced == 0)
		{
			stringTable[{1, 0}].textColor = constants::WarningTextColor;
		}

		return stringTable;
	}

	return StringTable(0, 0);
}


NAS2D::Dictionary Structure::getDataDict() const
{
	NAS2D::Dictionary dictionary =
	{{
		{"age", mAge},
		{"state", static_cast<int>(mStructureState)},
		{"forced_idle", mForcedIdle},
		{"disabled_reason", static_cast<int>(mDisabledReason)},
		{"idle_reason", static_cast<int>(mIdleReason)},
		{"type", static_cast<std::size_t>(mStructureId)},
		{"direction", static_cast<int>(mConnectorDirection)},
		{"integrity", mIntegrity},
		{"pop0", mPopulationAvailable.workers},
		{"pop1", mPopulationAvailable.scientists},
	}};

	if (hasCrime())
	{
		dictionary.set("crime_rate", mCrimeRate);
	}

	return dictionary;
}
