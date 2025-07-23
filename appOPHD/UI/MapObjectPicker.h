#pragma once

#include "IconGrid.h"

#include <libOPHD/EnumStructureID.h>

#include <libControls/ControlContainer.h>

#include <NAS2D/Signal/Delegate.h>


enum class RobotTypeIndex;
enum class InsertMode;
struct StorableResources;


class MapObjectPicker : public ControlContainer
{
public:
	using SelectionChangedDelegate = NAS2D::Delegate<void()>;

	MapObjectPicker(const StorableResources& resources, SelectionChangedDelegate selectionChangedHandler);
	~MapObjectPicker() override;

	IconGrid& structures() { return mStructures; }
	IconGrid& tubes() { return mConnections; }
	IconGrid& robots() { return mRobots; }

	bool isInserting() const;
	bool isInsertingStructure() const;
	bool isInsertingRobot() const;
	bool isInsertingTube() const;
	StructureID selectedStructureId() const;
	RobotTypeIndex selectedRobotIndex() const;
	void clearBuildMode();
	void clearSelections();

protected:
	void onStructuresSelectionChange(const IconGridItem*);
	void onConnectionsSelectionChange(const IconGridItem*);
	void onRobotsSelectionChange(const IconGridItem*);
	void onMouseWheel(NAS2D::Vector<int> changeAmount);
	void onResize() override;

public:
	const SelectionChangedDelegate mSelectionChangedHandler;
	const StorableResources& mResourcesCount;
	InsertMode mInsertMode; /**< What's being inserted into the TileMap if anything. */
	StructureID mCurrentStructure; /**< Structure being placed. */
	RobotTypeIndex mCurrentRobot; /**< Robot being placed. */
	IconGrid mStructures;
	IconGrid mRobots;
	IconGrid mConnections;
};
