#include <libOPHD/Technology/ResearchTracker.h>

#include <gtest/gtest.h>

#include <stdexcept>


TEST(ResearchTracker, IsCompleted)
{
	ResearchTracker tracker;
	EXPECT_FALSE(tracker.isCompleted(1000));

	tracker.addCompletedResearch(1000);
	EXPECT_TRUE(tracker.isCompleted(1000));
	EXPECT_FALSE(tracker.isCompleted(1010));
}


TEST(ResearchTracker, IsInProgress)
{
	ResearchTracker tracker;
	EXPECT_FALSE(tracker.isInProgress(1000));

	tracker.startResearch(1000, 0, 0);
	EXPECT_TRUE(tracker.isInProgress(1000));
	EXPECT_FALSE(tracker.isInProgress(1010));
}


TEST(ResearchTracker, PrerequisitesMetNoRequirements)
{
	ResearchTracker tracker;
	EXPECT_TRUE(tracker.prerequisitesMet(Technology{}));
}


TEST(ResearchTracker, PrerequisitesMet)
{
	Technology technology{};
	technology.requiredTechnologies = {1000, 5030};

	ResearchTracker tracker;
	EXPECT_FALSE(tracker.prerequisitesMet(technology));

	tracker.addCompletedResearch(1000);
	EXPECT_FALSE(tracker.prerequisitesMet(technology));

	tracker.startResearch(5030, 0, 0);
	EXPECT_FALSE(tracker.prerequisitesMet(technology));

	tracker.addCompletedResearch(5030);
	EXPECT_TRUE(tracker.prerequisitesMet(technology));
}


TEST(ResearchTracker, StartResearch)
{
	ResearchTracker tracker;
	tracker.startResearch(1000, 10, 2);

	EXPECT_EQ(10, tracker.researchProgress(1000).progress);
	EXPECT_EQ(2, tracker.researchProgress(1000).scientistsAssigned);
}


TEST(ResearchTracker, StartResearchOnCompletedThrows)
{
	ResearchTracker tracker;
	tracker.addCompletedResearch(1000);

	EXPECT_THROW(tracker.startResearch(1000, 0, 0), std::runtime_error);
}


TEST(ResearchTracker, UpdateResearch)
{
	ResearchTracker tracker;
	tracker.startResearch(1000, 0, 0);
	tracker.updateResearch(1000, 25, 1);

	EXPECT_EQ(25, tracker.researchProgress(1000).progress);
	EXPECT_EQ(1, tracker.researchProgress(1000).scientistsAssigned);
}


TEST(ResearchTracker, UpdateResearchNotStartedThrows)
{
	ResearchTracker tracker;
	EXPECT_THROW(tracker.updateResearch(1000, 0, 0), std::out_of_range);
}


TEST(ResearchTracker, AddProgressStartsResearch)
{
	Technology technology{};
	technology.id = 1000;
	technology.cost = 50;

	ResearchTracker tracker;
	EXPECT_FALSE(tracker.addProgress(technology, 10));
	EXPECT_TRUE(tracker.isInProgress(1000));
	EXPECT_EQ(10, tracker.researchProgress(1000).progress);
}


TEST(ResearchTracker, AddProgressAccumulates)
{
	Technology technology{};
	technology.id = 1000;
	technology.cost = 50;

	ResearchTracker tracker;
	tracker.addProgress(technology, 10);
	EXPECT_FALSE(tracker.addProgress(technology, 15));
	EXPECT_EQ(25, tracker.researchProgress(1000).progress);
}


TEST(ResearchTracker, AddProgressCompletesAtCost)
{
	Technology technology{};
	technology.id = 1000;
	technology.cost = 50;

	ResearchTracker tracker;
	tracker.addProgress(technology, 40);
	EXPECT_TRUE(tracker.addProgress(technology, 10));
	EXPECT_TRUE(tracker.isCompleted(1000));
	EXPECT_FALSE(tracker.isInProgress(1000));
}


TEST(ResearchTracker, AddProgressCompletesOverCost)
{
	Technology technology{};
	technology.id = 1000;
	technology.cost = 50;

	ResearchTracker tracker;
	EXPECT_TRUE(tracker.addProgress(technology, 75));
	EXPECT_TRUE(tracker.isCompleted(1000));
	EXPECT_FALSE(tracker.isInProgress(1000));
}


TEST(ResearchTracker, AddProgressOnCompletedThrows)
{
	Technology technology{};
	technology.id = 1000;
	technology.cost = 50;

	ResearchTracker tracker;
	tracker.addCompletedResearch(1000);

	EXPECT_THROW(tracker.addProgress(technology, 10), std::runtime_error);
}


TEST(ResearchTracker, ActiveResearchNotSet)
{
	ResearchTracker tracker;
	EXPECT_FALSE(tracker.activeResearch(LabType::Standard).has_value());
	EXPECT_FALSE(tracker.activeResearch(LabType::Hot).has_value());
}


TEST(ResearchTracker, SetActiveResearch)
{
	Technology technology{};
	technology.id = 1000;
	technology.labType = LabType::Standard;

	ResearchTracker tracker;
	tracker.setActiveResearch(technology);

	EXPECT_EQ(1000, tracker.activeResearch(LabType::Standard));
	EXPECT_FALSE(tracker.activeResearch(LabType::Hot).has_value());
}


TEST(ResearchTracker, SetActiveResearchPerLabType)
{
	Technology standardTechnology{};
	standardTechnology.id = 1000;
	standardTechnology.labType = LabType::Standard;

	Technology hotTechnology{};
	hotTechnology.id = 1100;
	hotTechnology.labType = LabType::Hot;

	ResearchTracker tracker;
	tracker.setActiveResearch(standardTechnology);
	tracker.setActiveResearch(hotTechnology);

	EXPECT_EQ(1000, tracker.activeResearch(LabType::Standard));
	EXPECT_EQ(1100, tracker.activeResearch(LabType::Hot));
}


TEST(ResearchTracker, SetActiveResearchReplacesAndKeepsProgress)
{
	Technology firstTechnology{};
	firstTechnology.id = 1000;
	firstTechnology.labType = LabType::Standard;
	firstTechnology.cost = 50;

	Technology secondTechnology{};
	secondTechnology.id = 1020;
	secondTechnology.labType = LabType::Standard;

	ResearchTracker tracker;
	tracker.setActiveResearch(firstTechnology);
	tracker.addProgress(firstTechnology, 10);
	tracker.setActiveResearch(secondTechnology);

	EXPECT_EQ(1020, tracker.activeResearch(LabType::Standard));
	EXPECT_EQ(10, tracker.researchProgress(1000).progress);
}


TEST(ResearchTracker, SetActiveResearchOnCompletedThrows)
{
	Technology technology{};
	technology.id = 1000;

	ResearchTracker tracker;
	tracker.addCompletedResearch(1000);

	EXPECT_THROW(tracker.setActiveResearch(technology), std::runtime_error);
}


TEST(ResearchTracker, SetActiveResearchWithUnmetPrerequisitesThrows)
{
	Technology technology{};
	technology.id = 1010;
	technology.requiredTechnologies = {1000};

	ResearchTracker tracker;
	EXPECT_THROW(tracker.setActiveResearch(technology), std::runtime_error);
	EXPECT_FALSE(tracker.activeResearch(technology.labType).has_value());

	tracker.addCompletedResearch(1000);
	EXPECT_NO_THROW(tracker.setActiveResearch(technology));
	EXPECT_EQ(1010, tracker.activeResearch(technology.labType));
}


TEST(ResearchTracker, AddProgressCompletingActiveResearchClearsIt)
{
	Technology technology{};
	technology.id = 1000;
	technology.labType = LabType::Standard;
	technology.cost = 50;

	ResearchTracker tracker;
	tracker.setActiveResearch(technology);
	tracker.addProgress(technology, 50);

	EXPECT_FALSE(tracker.activeResearch(LabType::Standard).has_value());
}


TEST(ResearchTracker, AddProgressCompletingOtherResearchKeepsActive)
{
	Technology activeTechnology{};
	activeTechnology.id = 1000;
	activeTechnology.labType = LabType::Standard;

	Technology otherTechnology{};
	otherTechnology.id = 1020;
	otherTechnology.labType = LabType::Standard;
	otherTechnology.cost = 50;

	ResearchTracker tracker;
	tracker.setActiveResearch(activeTechnology);
	tracker.addProgress(otherTechnology, 50);

	EXPECT_EQ(1000, tracker.activeResearch(LabType::Standard));
}
