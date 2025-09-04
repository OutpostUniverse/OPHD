#pragma once

#include "IconGrid.h"

#include <libControls/ControlContainer.h>

#include <NAS2D/Signal/Delegate.h>

#include <vector>


enum class StructureID;
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

	void setStructureIds(const std::vector<StructureID>& structureIds);
	void setTubesAboveGround();
	void setTubesUnderGround();

	void addStructure(StructureID structureId);

	bool isInserting() const;
	bool isInsertingStructure() const;
	bool isInsertingRobot() const;
	bool isInsertingTube() const;
	StructureID selectedStructureId() const;
	RobotTypeIndex selectedRobotIndex() const;
	void clearBuildMode();
	void clearSelections();

	using ControlContainer::update;

protected:
	void onStructuresSelectionChange(const IconGridItem* item);
	void onConnectionsSelectionChange(const IconGridItem* item);
	void onRobotsSelectionChange(const IconGridItem* item);
	void onResize() override;

public:
	const SelectionChangedDelegate mSelectionChangedHandler;
	const StorableResources& mResourcesCount;
	InsertMode mInsertMode;
	StructureID mCurrentStructure;
	RobotTypeIndex mCurrentRobot;
	IconGrid mStructures;
	IconGrid mRobots;
	IconGrid mConnections;
};
