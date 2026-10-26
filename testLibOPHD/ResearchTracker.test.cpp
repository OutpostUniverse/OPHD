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
