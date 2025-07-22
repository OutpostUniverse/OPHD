#include "MapViewState.h"

#include "MapViewStateHelper.h"
#include "../MapObjects/RobotTypeIndex.h"


bool MapViewState::isInserting() const
{
	return mInsertMode != InsertMode::None;
}


bool MapViewState::isInsertingStructure() const
{
	return mInsertMode == InsertMode::Structure;
}


bool MapViewState::isInsertingRobot() const
{
	return mInsertMode == InsertMode::Robot;
}


void MapViewState::clearBuildMode()
{
	mInsertMode = InsertMode::None;
	mCurrentStructure = StructureID::SID_NONE;
	mCurrentRobot = RobotTypeIndex::None;
	onMapObjectSelectionChanged();
}


void MapViewState::clearSelections()
{
	mStructures.clearSelection();
	mConnections.clearSelection();
	mRobots.clearSelection();
}


void MapViewState::onMouseWheel(NAS2D::Vector<int> changeAmount)
{
	if (mInsertMode != InsertMode::Tube) { return; }

	changeAmount.y > 0 ? mConnections.decrementSelection() : mConnections.incrementSelection();
}


void MapViewState::onStructuresSelectionChange(const IconGridItem* item)
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
	onMapObjectSelectionChanged();
}


/**
 * Handler for the Tubes Pallette dialog.
 */
void MapViewState::onConnectionsSelectionChange(const IconGridItem* item)
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
	onMapObjectSelectionChanged();
}


/**
 * Handles clicks of the Robot Selection Menu.
 */
void MapViewState::onRobotsSelectionChange(const IconGridItem* item)
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
	onMapObjectSelectionChanged();
}
