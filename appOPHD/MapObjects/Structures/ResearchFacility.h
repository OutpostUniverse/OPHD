#pragma once

#include "../Structure.h"


class ResearchFacility : public Structure
{
public:
	ResearchFacility(StructureClass structureClass, StructureID id, Tile& tile);

	StringTable createInspectorViewTable() const override;

	int regularResearchProduced() const;
	int hotResearchProduced() const;

	int scientistsNeeded() const;

	void assignScientists(int count);
	int assignedScientists() const;

protected:
	void maxScientistsAllowed(int count);

	void regularPointsPerScientist(float count);
	void hotPointsPerScientist(float count);

private:
	int mMaxScientistsAllowed{0};
	int mActualScientistsEmployed{0};

	float mRegularPointsPerScientist{0.0f};
	float mHotPointsPerScientist{0.0f};
};
