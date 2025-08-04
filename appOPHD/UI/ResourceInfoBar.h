#pragma once

#include <libControls/ControlContainer.h>
#include <libControls/ToolTip.h>

#include <NAS2D/Math/Point.h>
#include <NAS2D/Resource/Image.h>


namespace NAS2D
{
	enum class MouseButton;
}

enum class StructureClass;
struct StorableResources;
class StructureManager;
class Population;
class Morale;


class ResourceInfoBar : public ControlContainer
{
public:
	ResourceInfoBar(const StorableResources& resources, const Population& population, const Morale& morale, const int& food);
	~ResourceInfoBar() override;

	bool isResourcePanelVisible() const;
	bool isPopulationPanelVisible() const;

	void ignoreGlow(const bool ignore);

	void update() override;
	void draw(NAS2D::Renderer& renderer) const override;

protected:
	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);

private:
	const StorableResources& mResourcesCount;
	const StructureManager& mStructureManager;
	const Population& mPopulation;
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
	bool mIgnoreGlow = false;
};
