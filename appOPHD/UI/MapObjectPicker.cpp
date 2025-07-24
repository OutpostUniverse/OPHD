#include "MapObjectPicker.h"

#include "../States/MapViewStateHelper.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../MapObjects/RobotTypeIndex.h"
#include "../StructureCatalog.h"

#include <libOPHD/EnumStructureID.h>
#include <libOPHD/EnumConnectorDir.h>
#include <libOPHD/MapObjects/StructureType.h>

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>


#include <array>


namespace
{
	constexpr auto NotSet = int{54};

	constexpr auto structureIconTable = std::array{
		NotSet, // SID_NONE
		5, // SID_AGRIDOME
		NotSet, // SID_AIR_SHAFT
		1, // SID_CARGO_LANDER
		3, // SID_CHAP
		2, // SID_COLONIST_LANDER
		NotSet, // SID_COMMAND_CENTER
		66, // SID_COMMERCIAL
		22, // SID_COMM_TOWER
		21, // SID_FUSION_REACTOR
		18, // SID_HOT_LABORATORY
		58, // SID_LABORATORY
		62, // SID_MEDICAL_CENTER
		NotSet, // SID_MINE_FACILITY
		NotSet, // SID_MINE_SHAFT
		77, // SID_NURSERY
		75, // SID_PARK
		73, // SID_RECREATION_CENTER
		76, // SID_RED_LIGHT_DISTRICT
		55, // SID_RESIDENCE
		24, // SID_ROAD
		14, // SID_ROBOT_COMMAND
		98, // SID_SEED_FACTORY
		0, // SID_SEED_LANDER
		98, // SID_SEED_POWER
		98, // SID_SEED_SMELTER
		4, // SID_SMELTER
		33, // SID_SOLAR_PANEL1
		10, // SID_SOLAR_PLANT
		8, // SID_STORAGE_TANKS
		11, // SID_SURFACE_FACTORY
		23, // SID_SURFACE_POLICE
		110, // SID_TUBE
		69, // SID_UNDERGROUND_FACTORY
		61, // SID_UNDERGROUND_POLICE
		63, // SID_UNIVERSITY
		9, // SID_WAREHOUSE
		16, // SID_RECYCLING
		54, // SID_MAINTENANCE_FACILITY
	};

	static_assert(structureIconTable.size() == static_cast<std::size_t>(StructureID::SID_COUNT));


	IconGridItem idToIconGridItem(StructureID structureId)
	{
		const auto index = StructureCatalog::typeIndex(structureId);
		return {
			StructureCatalog::getType(index).name,
			structureIconTable.at(index),
			static_cast<int>(index),
		};
	}


	const std::vector<IconGridItem> SurfaceTubes = {
		{constants::AgTubeIntersection, 110, ConnectorDir::CONNECTOR_INTERSECTION},
		{constants::AgTubeRight, 112, ConnectorDir::CONNECTOR_EAST_WEST},
		{constants::AgTubeLeft, 111, ConnectorDir::CONNECTOR_NORTH_SOUTH},
	};

	const std::vector<IconGridItem> UndergroundTubes = {
		{constants::UgTubeIntersection, 113, ConnectorDir::CONNECTOR_INTERSECTION},
		{constants::UgTubeRight, 115, ConnectorDir::CONNECTOR_EAST_WEST},
		{constants::UgTubelLeft, 114, ConnectorDir::CONNECTOR_NORTH_SOUTH},
	};


	void setTubes(IconGrid& tubesGrid, const std::vector<IconGridItem>& items)
	{
		tubesGrid.clear();
		for (const auto& item : items)
		{
			tubesGrid.addItem(item);
		}
	}
}


enum class InsertMode
{
	None,
	Robot,
	Tube,
	Structure
};


MapObjectPicker::MapObjectPicker(const StorableResources& resources, SelectionChangedDelegate selectionChangedHandler) :
	ControlContainer{{&mStructures, &mConnections, &mRobots}},
	mSelectionChangedHandler{selectionChangedHandler},
	mResourcesCount{resources},
	mInsertMode{InsertMode::None},
	mCurrentStructure{StructureID::SID_NONE},
	mCurrentRobot{RobotTypeIndex::None},
	mStructures{{this, &MapObjectPicker::onStructuresSelectionChange}, "ui/structures.png", constants::StructureIconSize, constants::MarginTight, true},
	mRobots{{this, &MapObjectPicker::onRobotsSelectionChange}, "ui/robots.png", constants::RobotIconSize, constants::MarginTight, true},
	mConnections{{this, &MapObjectPicker::onConnectionsSelectionChange}, "ui/structures.png", constants::StructureIconSize, constants::MarginTight}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseWheel().connect({this, &MapObjectPicker::onMouseWheel});
}


MapObjectPicker::~MapObjectPicker()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseWheel().disconnect({this, &MapObjectPicker::onMouseWheel});
}


void MapObjectPicker::setStructureIds(const std::vector<StructureID>& structureIds)
{
	mStructures.clear();
	for (const auto structureId : structureIds)
	{
		mStructures.addItem(idToIconGridItem(structureId));
	}
}


void MapObjectPicker::setTubesAboveGround()
{
	setTubes(mConnections, SurfaceTubes);
}


void MapObjectPicker::setTubesUnderGround()
{
	setTubes(mConnections, UndergroundTubes);
}


void MapObjectPicker::addStructure(StructureID structureId)
{
	mStructures.addItem(idToIconGridItem(structureId));
}


bool MapObjectPicker::isInserting() const
{
	return mInsertMode != InsertMode::None;
}


bool MapObjectPicker::isInsertingStructure() const
{
	return mInsertMode == InsertMode::Structure;
}


bool MapObjectPicker::isInsertingRobot() const
{
	return mInsertMode == InsertMode::Robot;
}


bool MapObjectPicker::isInsertingTube() const
{
	return mInsertMode == InsertMode::Tube;
}


StructureID MapObjectPicker::selectedStructureId() const
{
	return mCurrentStructure;
}


RobotTypeIndex MapObjectPicker::selectedRobotIndex() const
{
	return mCurrentRobot;
}


void MapObjectPicker::clearBuildMode()
{
	mInsertMode = InsertMode::None;
	mCurrentStructure = StructureID::SID_NONE;
	mCurrentRobot = RobotTypeIndex::None;
	mSelectionChangedHandler();
}


void MapObjectPicker::clearSelections()
{
	mStructures.clearSelection();
	mConnections.clearSelection();
	mRobots.clearSelection();
}


void MapObjectPicker::onMouseWheel(NAS2D::Vector<int> changeAmount)
{
	if (mInsertMode != InsertMode::Tube) { return; }

	changeAmount.y > 0 ? mConnections.decrementSelection() : mConnections.incrementSelection();
}


void MapObjectPicker::onStructuresSelectionChange(const IconGridItem* item)
{
	if (!item)
	{
		clearBuildMode();
		return;
	}

	mConnections.clearSelection();
	mRobots.clearSelection();

	const auto structureId = static_cast<StructureID>(item->meta);

	// Check availability
	if (!item->available)
	{
		resourceShortageMessage(mResourcesCount, structureId);
		mStructures.clearSelection();
		return;
	}

	mCurrentStructure = structureId;
	mInsertMode = InsertMode::Structure;
	mSelectionChangedHandler();
}


/**
 * Handler for the Tubes Pallette dialog.
 */
void MapObjectPicker::onConnectionsSelectionChange(const IconGridItem* item)
{
	if (!item)
	{
		clearBuildMode();
		return;
	}

	mRobots.clearSelection();
	mStructures.clearSelection();

	mCurrentStructure = StructureID::SID_TUBE;
	mInsertMode = InsertMode::Tube;
	mSelectionChangedHandler();
}


/**
 * Handles clicks of the Robot Selection Menu.
 */
void MapObjectPicker::onRobotsSelectionChange(const IconGridItem* item)
{
	if (!item)
	{
		clearBuildMode();
		return;
	}

	mConnections.clearSelection();
	mStructures.clearSelection();

	mCurrentRobot = static_cast<RobotTypeIndex>(item->meta);
	mInsertMode = InsertMode::Robot;
	mSelectionChangedHandler();
}


void MapObjectPicker::onResize()
{
	mRobots.position(mRect.crossXPoint() - NAS2D::Vector{52, 0});
	mConnections.position({mRobots.position().x - constants::MarginTight - 52, mRect.position.y});
	mStructures.position(mRect.position);

	mRobots.size({52, mRect.size.y});
	mConnections.size({52, mRect.size.y});
	mStructures.size({mRect.size.x - 52 * 2 - constants::MarginTight * 2, mRect.size.y});
}
