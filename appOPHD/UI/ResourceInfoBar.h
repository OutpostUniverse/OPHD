#pragma once

#include <libControls/ControlContainer.h>
#include <libControls/ToolTip.h>


namespace NAS2D
{
	enum class MouseButton;
	class Image;
	template <typename BaseType> struct Point;
	template <typename BaseType> struct Vector;
}

struct StorableResources;
class StructureManager;
class PopulationModel;
class Morale;


class ResourceInfoBar : public ControlContainer
{
public:
	ResourceInfoBar(const StorableResources& resources, const StructureManager& structureManager, const PopulationModel& population, const Morale& morale, const int& food);
	~ResourceInfoBar() override;

	bool isResourcePanelVisible() const;
	bool isPopulationPanelVisible() const;

	void ignoreGlow(const bool ignore);

	void update() override;
	void draw(NAS2D::Renderer& renderer) const override;

protected:
	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

private:
	const StorableResources& mResourcesCount;
	const StructureManager& mStructureManager;
	const PopulationModel& mPopulationModel;
	const Morale& mMorale;
	const int& mFood;

	const NAS2D::Image& mUiIcons;

	// Bare Control's use for ToolTips
	Control mTooltipPopulation;
	Control mTooltipEnergy;
	Control mTooltipFoodStorage;
	Control mTooltipResourceStorage;
	Control mTooltipResourceBreakdown;

	ToolTip mToolTip;

	bool mPinResourcePanel = false;
	bool mPinPopulationPanel = false;
	bool mHoverResourcePanel = false;
	bool mHoverPopulationPanel = false;
	bool mIgnoreGlow = false;
};
