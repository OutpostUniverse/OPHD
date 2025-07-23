#include "MapObjectPicker.h"

#include "../States/MapViewStateHelper.h"
#include "../Constants/UiConstants.h"
#include "../MapObjects/RobotTypeIndex.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>


enum class InsertMode
{
	None,
	Robot,
	Tube,
	Structure
};


MapObjectPicker::MapObjectPicker(const StorableResources& resources, SelectionChangedDelegate selectionChangedHandler) :
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
