#include "Structure.h"

#include "../../Constants.h"


/**
 * Translation table for Structure States.
 */
const std::map<StructureState, std::string> STRUCTURE_STATE_TRANSLATION =
{
	{ StructureState::UnderConstruction, "Under Construction" },
	{ StructureState::Operational, "Operational" },
	{ StructureState::Idle, "Idle" },
	{ StructureState::Disabled, "Disabled" },
	{ StructureState::Destroyed, "Destroyed" },
};


/**
 * Translation table for Structure Classes.
 */
const std::map<Structure::StructureClass, std::string> STRUCTURE_CLASS_TRANSLATION =
{
	{ Structure::StructureClass::Command, "Command" },
	{ Structure::StructureClass::Communication, "Communication" },
	{ Structure::StructureClass::Commercial, "Commercial" },
	{ Structure::StructureClass::EnergyProduction, "Energy Production" },
	{ Structure::StructureClass::Factory, "Factory" },
	{ Structure::StructureClass::FoodProduction, "Food Production" },
	{ Structure::StructureClass::Laboratory, "Laboratory" },
	{ Structure::StructureClass::Lander, "Lander" },
	{ Structure::StructureClass::LifeSupport, "Life Support" },
	{ Structure::StructureClass::Mine, "Mine Facility" },
	{ Structure::StructureClass::MedicalCenter, "Mine Facility" },
	{ Structure::StructureClass::Nursery, "Mine Facility" },
	{ Structure::StructureClass::Park, "Park / Reservoir" },
	{ Structure::StructureClass::Road, "Road" },
	{ Structure::StructureClass::SurfacePolice, "Police" },
	{ Structure::StructureClass::UndergroundPolice, "Police" },
	{ Structure::StructureClass::RecreationCenter, "Recreation Center" },
	{ Structure::StructureClass::Recycling, "Recycling" },
	{ Structure::StructureClass::Residence, "Residential" },
	{ Structure::StructureClass::RobotCommand, "Robot Command Center" },
	{ Structure::StructureClass::Smelter, "Raw Ore Processing" },
	{ Structure::StructureClass::Storage, "Storage" },
	{ Structure::StructureClass::Tube, "Tube" },
	{ Structure::StructureClass::Undefined, "UNDEFINED" },
	{ Structure::StructureClass::University, "University" },
	{ Structure::StructureClass::Warehouse, "Warehouse"}
};


static const std::array<std::string, StructureID::SID_COUNT> StructureNameTable =
{
	"Not a Structure",
	constants::AGRIDOME,
	constants::AIR_SHAFT,
	constants::CARGO_LANDER,
	constants::CHAP,
	constants::COLONIST_LANDER,
	constants::COMMAND_CENTER,
	constants::COMMERCIAL,
	constants::COMM_TOWER,
	constants::FUSION_REACTOR,
	constants::HOT_LABORATORY,
	constants::LABORATORY,
	constants::MEDICAL_CENTER,
	constants::MINE_FACILITY,
	constants::MINE_SHAFT,
	constants::NURSERY,
	constants::PARK,
	constants::RECREATION_CENTER,
	constants::RED_LIGHT_DISTRICT,
	constants::RESIDENCE,
	constants::ROAD,
	constants::ROBOT_COMMAND,
	constants::SEED_FACTORY,
	constants::SEED_LANDER,
	constants::SEED_POWER,
	constants::SEED_SMELTER,
	constants::SMELTER,
	constants::SOLAR_PANEL1,
	constants::SOLAR_PLANT,
	constants::STORAGE_TANKS,
	constants::SURFACE_FACTORY,
	constants::SURFACE_POLICE,
	constants::TUBE,
	constants::UNDERGROUND_FACTORY,
	constants::UNDERGROUND_POLICE,
	constants::UNIVERSITY,
	constants::WAREHOUSE
};


std::string StructureName(StructureID id)
{
	return StructureNameTable[static_cast<size_t>(id)];
}



Structure::Structure(const std::string& name, const std::string& spritePath, StructureClass structureClass, StructureID id) :
	Thing(name, spritePath, constants::STRUCTURE_STATE_CONSTRUCTION),
	mStructureId(id),
	mStructureClass(structureClass)
{
	mPopulationRequirements.fill(0);
	mPopulationAvailable.fill(0);
}


Structure::Structure(const std::string& name, const std::string& spritePath, const std::string& initialAction, StructureClass structureClass, StructureID id) :
	Thing(name, spritePath, initialAction),
	mStructureId(id),
	mStructureClass(structureClass)
{
	mPopulationRequirements.fill(0);
	mPopulationAvailable.fill(0);
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
	return classDescription(structureClass());
}

const std::string& Structure::classDescription(Structure::StructureClass structureClass)
{
	return STRUCTURE_CLASS_TRANSLATION.at(structureClass);
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
	sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
	enable();

	defineResourceInput();

	activated();
}


void Structure::update()
{
	if (disabled() || destroyed()) { return; }
	incrementAge();
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
	else if (age() == maxAge())
	{
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
	sprite().play(constants::STRUCTURE_STATE_DESTROYED);
	state(StructureState::Destroyed);

	// Destroyed buildings just need to be rebuilt right?
	repairable(false);
}


/**
 * Provided for loading purposes.
 */
void Structure::forced_state_change(StructureState structureState, DisabledReason disabledReason, IdleReason idleReason)
{
	defineResourceInput();

	if (age() >= turnsToBuild())
	{
		sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
		//enable();
	}

	if (structureState == StructureState::Operational) { enable(); }
	else if (structureState == StructureState::Idle) { idle(idleReason); }
	else if (structureState == StructureState::Disabled) { disable(disabledReason); }
	else if (structureState == StructureState::Destroyed) { destroy(); }
	else if (structureState == StructureState::UnderConstruction) { mStructureState = StructureState::UnderConstruction; } // Kludge
}


/**
 * Special overidding of Thing::die for Structure.
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
	Thing::die();

	throw std::runtime_error("Thing::die() was called on a Structure!");
}
