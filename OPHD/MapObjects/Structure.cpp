#include "Structure.h"

#include "StructureType.h"

#include "../StructureCatalogue.h"
#include "../Constants/Strings.h"

#include <libOPHD/RandomNumberGenerator.h>

#include <algorithm>


/**
 * Translation table for Structure States.
 */
const std::map<StructureState, std::string> STRUCTURE_STATE_TRANSLATION =
{
	{StructureState::UnderConstruction, "Under Construction"},
	{StructureState::Operational, "Operational"},
	{StructureState::Idle, "Idle"},
	{StructureState::Disabled, "Disabled"},
	{StructureState::Destroyed, "Destroyed"},
};


/**
 * Translation table for Structure Classes.
 */
const std::map<Structure::StructureClass, std::string> STRUCTURE_CLASS_TRANSLATION =
{
	{Structure::StructureClass::Command, "Command"},
	{Structure::StructureClass::Communication, "Communication"},
	{Structure::StructureClass::Commercial, "Commercial"},
	{Structure::StructureClass::EnergyProduction, "Energy Production"},
	{Structure::StructureClass::Factory, "Factory"},
	{Structure::StructureClass::FoodProduction, "Food Production"},
	{Structure::StructureClass::Laboratory, "Laboratory"},
	{Structure::StructureClass::Lander, "Lander"},
	{Structure::StructureClass::LifeSupport, "Life Support"},
	{Structure::StructureClass::Maintenance, "Maintenance Facility"},
	{Structure::StructureClass::Mine, "Mine Facility"},
	{Structure::StructureClass::MedicalCenter, "Mine Facility"},
	{Structure::StructureClass::Nursery, "Mine Facility"},
	{Structure::StructureClass::Park, "Park / Reservoir"},
	{Structure::StructureClass::Road, "Road"},
	{Structure::StructureClass::SurfacePolice, "Police"},
	{Structure::StructureClass::UndergroundPolice, "Police"},
	{Structure::StructureClass::RecreationCenter, "Recreation Center"},
	{Structure::StructureClass::Recycling, "Recycling"},
	{Structure::StructureClass::Residence, "Residential"},
	{Structure::StructureClass::RobotCommand, "Robot Command Center"},
	{Structure::StructureClass::Smelter, "Raw Ore Processing"},
	{Structure::StructureClass::Storage, "Storage"},
	{Structure::StructureClass::Tube, "Tube"},
	{Structure::StructureClass::Undefined, "UNDEFINED"},
	{Structure::StructureClass::University, "University"},
	{Structure::StructureClass::Warehouse, "Warehouse"}
};


static const std::array<std::string, StructureID::SID_COUNT> StructureNameTable =
{
	"Not a Structure",
	constants::Agridome,
	constants::AirShaft,
	constants::CargoLander,
	constants::Chap,
	constants::ColonistLander,
	constants::CommandCenter,
	constants::Commercial,
	constants::CommTower,
	constants::FusionReactor,
	constants::HotLaboratory,
	constants::Laboratory,
	constants::MedicalCenter,
	constants::MineFacility,
	constants::MineShaft,
	constants::Nursery,
	constants::Park,
	constants::RecreationCenter,
	constants::RedLightDistrict,
	constants::Residence,
	constants::Road,
	constants::RobotCommand,
	constants::SeedFactory,
	constants::SeedLander,
	constants::SeedPower,
	constants::SeedSmelter,
	constants::Smelter,
	constants::SolarPanel1,
	constants::SolarPlant,
	constants::StorageTanks,
	constants::SurfaceFactory,
	constants::SurfacePolice,
	constants::Tube,
	constants::UndergroundFactory,
	constants::UndergroundPolice,
	constants::University,
	constants::Warehouse,
	constants::Recycling,
	constants::MaintenanceFacility
};


std::string StructureName(StructureID id)
{
	return StructureNameTable[static_cast<size_t>(id)];
}


std::vector<Structure::StructureClass> allStructureClasses()
{
	std::vector<Structure::StructureClass> result;
	for (const auto& entry : STRUCTURE_CLASS_TRANSLATION)
	{
		result.push_back(entry.first);
	}
	return result;
}


Structure::Structure(StructureClass structureClass, StructureID id) :
	MapObject(StructureName(id), StructureCatalogue::getType(id).spritePath, constants::StructureStateConstruction),
	mStructureType(StructureCatalogue::getType(id)),
	mStructureId(id),
	mStructureClass(structureClass)
{
}


Structure::Structure(const std::string& initialAction, StructureClass structureClass, StructureID id) :
	MapObject(StructureName(id), StructureCatalogue::getType(id).spritePath, initialAction),
	mStructureType(StructureCatalogue::getType(id)),
	mStructureId(id),
	mStructureClass(structureClass)
{
}


/**
 * Sets a Disabled state for the Structure.
 */
void Structure::disable(DisabledReason reason)
{
	sprite().pause();
	sprite().color(NAS2D::Color{255, 0, 0, 185});
	state(StructureState::Disabled);
	mDisabledReason = reason;
	mIdleReason = IdleReason::None;
	disabledStateSet();
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

	sprite().resume();
	sprite().color(NAS2D::Color::White);
	state(StructureState::Operational);
	mDisabledReason = DisabledReason::None;
	mIdleReason = IdleReason::None;
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

	sprite().pause();
	sprite().color(NAS2D::Color{255, 255, 255, 185});
	mDisabledReason = DisabledReason::None;
	mIdleReason = reason;
	state(StructureState::Idle);
}


/**
 * Forces an idle state. Used to prevent automatic enabling of the
 * Structure.
 */
void Structure::forceIdle(bool force)
{
	if (disabled() || destroyed()) { return; }

	// Note that the order in which the flag is set matters
	// in terms of the logic involved here.
	if (force)
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

const std::string& Structure::stateDescription() const
{
	return stateDescription(state());
}

const std::string& Structure::stateDescription(StructureState state)
{
	return STRUCTURE_STATE_TRANSLATION.at(state);
}

const std::string& Structure::classDescription() const
{
	return classDescription(mStructureClass);
}

const std::string& Structure::classDescription(StructureClass structureClass)
{
	return STRUCTURE_CLASS_TRANSLATION.at(structureClass);
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

int Structure::storageCapacity() const
{
	return mStructureType.oreStorageCapacity;
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

/**
 * Called when a building is finished being built.
 *
 * Sets the animation state of the Structure to Operational,
 * sets the building state to Opeational and sets resource
 * requirements.
 */
void Structure::activate()
{
	sprite().play(constants::StructureStateOperational);
	enable();

	activated();
}


void Structure::rebuild()
{
	sprite().play(constants::StructureStateConstruction);
	state(StructureState::UnderConstruction);

	age(1);
}


void Structure::update()
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
	sprite().play(constants::StructureStateDestroyed);
	state(StructureState::Destroyed);
}


/**
 * Provided for loading purposes.
 */
void Structure::forced_state_change(StructureState structureState, DisabledReason disabledReason, IdleReason idleReason)
{
	if (age() >= turnsToBuild())
	{
		sprite().play(constants::StructureStateOperational);
		//enable();
	}

	if (structureState == StructureState::Operational) { enable(); }
	else if (structureState == StructureState::Idle) { idle(idleReason); }
	else if (structureState == StructureState::Disabled) { disable(disabledReason); }
	else if (structureState == StructureState::Destroyed) { destroy(); }
	else if (structureState == StructureState::UnderConstruction) { mStructureState = StructureState::UnderConstruction; } // Kludge
}


/**
 * Special overidding of MapObject::die for Structure.
 *
 * There is no conceivable situation in which a Structure should be marked as 'dead' or have its
 * 'die' function be called. Such cases should be treated as bad logic and immediately and very
 * loudly fail.
 *
 * This function exists purely for debug purposes as it was noticed in StructureManager testing
 * for this flag when there should never be a need to do so.
 *
 * \note	This is for debug purposes only. Release modes will silently ignore this condition
 *			and simply act as a passthrough.
 *
 * \throws	Throws \c std::runtime_error
 */
void Structure::die()
{
	MapObject::die();

	throw std::runtime_error("MapObject::die() was called on a Structure!");
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


NAS2D::Dictionary Structure::getDataDict() const
{
	NAS2D::Dictionary dictionary =
	{{
		{"age", mAge},
		{"state", static_cast<int>(mStructureState)},
		{"forced_idle", mForcedIdle},
		{"disabled_reason", static_cast<int>(mDisabledReason)},
		{"idle_reason", static_cast<int>(mIdleReason)},
		{"type", mStructureId},
		{"direction", mConnectorDirection},
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
