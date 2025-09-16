#include "ResourceInfoBar.h"

#include "../CacheImage.h"
#include "../Resources.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../States/MapViewStateHelper.h"

#include "../StructureManager.h"

#include <libOPHD/StorableResources.h>
#include <libOPHD/Population/PopulationModel.h>
#include <libOPHD/Population/Morale.h>

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Timer.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>

#include <cstdint>
#include <algorithm>


extern NAS2D::Point<int> MOUSE_COORDS;


namespace
{
	constexpr auto pinIconSize = NAS2D::Vector{8, 19};
	constexpr auto resourcePanelPinRect = NAS2D::Rectangle{{0, 1}, pinIconSize};
	constexpr auto populationPanelPinRect = NAS2D::Rectangle{{675, 1}, pinIconSize};

	constexpr auto iconSize = NAS2D::Vector{constants::ResourceIconSize, constants::ResourceIconSize};
	constexpr auto oreIconRect = NAS2D::Rectangle{{96, 32}, iconSize};
	constexpr auto foodIconRect = NAS2D::Rectangle{{64, 32}, iconSize};
	constexpr auto powerIconRect = NAS2D::Rectangle{{80, 32}, iconSize};


	uint8_t calcGlowIntensity()
	{
		static NAS2D::Timer glowTimer;
		static int glowStepDelta = 20;
		static int glowStep = 0;

		if (glowTimer.elapsedTicks() >= 10)
		{
			glowTimer.reset();

			glowStep += glowStepDelta;
			if (glowStep >= 255)
			{
				glowStep = 255;
				glowStepDelta = -glowStepDelta;
			}
			else if (glowStep <= 0)
			{
				glowStep = 0;
				glowStepDelta = -glowStepDelta;
			}
		}
		return static_cast<uint8_t>(glowStep);
	}
}


ResourceInfoBar::ResourceInfoBar(const StorableResources& resources, const StructureManager& structureManager, const PopulationModel& population, const Morale& morale, const int& food) :
	ControlContainer{{&mToolTip}},
	mResourcesCount{resources},
	mStructureManager{structureManager},
	mPopulationModel{population},
	mMorale{morale},
	mFood{food},
	mUiIcons{getImage("ui/icons.png")}
{
	const auto fontHeight = Control::getDefaultFont().height();
	const auto contentHeight = std::max(fontHeight, constants::ResourceIconSize);
	height(contentHeight + constants::MarginTight * 2);

	mTooltipResourceBreakdown.size({265, mRect.size.y});

	mTooltipResourceStorage.position({275, 0});
	mTooltipResourceStorage.size({85, mRect.size.y});

	mTooltipFoodStorage.position({420, 0});
	mTooltipFoodStorage.size({75, mRect.size.y});

	mTooltipEnergy.position({560, 0});
	mTooltipEnergy.size({55, mRect.size.y});

	mTooltipPopulation.position({670, 0});
	mTooltipPopulation.size({75, mRect.size.y});

	// Tool Tips
	mToolTip.add(mTooltipResourceBreakdown, constants::ToolTipRefinedResources);
	mToolTip.add(mTooltipResourceStorage, constants::ToolTipResourceStorage);
	mToolTip.add(mTooltipFoodStorage, constants::ToolTipFoodStorage);
	mToolTip.add(mTooltipEnergy, constants::ToolTipEnergy);
	mToolTip.add(mTooltipPopulation, constants::ToolTipPopulation);

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().connect({this, &ResourceInfoBar::onMouseDown});
}


ResourceInfoBar::~ResourceInfoBar()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect({this, &ResourceInfoBar::onMouseDown});
}


bool ResourceInfoBar::isResourcePanelVisible() const
{
	return mPinResourcePanel || NAS2D::Rectangle<int>{{0, 1}, {270, 19}}.contains(MOUSE_COORDS);
}


bool ResourceInfoBar::isPopulationPanelVisible() const
{
	return mPinPopulationPanel || NAS2D::Rectangle<int>{{675, 1}, {75, 19}}.contains(MOUSE_COORDS);
}

void ResourceInfoBar::ignoreGlow(const bool ignore)
{
	mIgnoreGlow = ignore;
}


void ResourceInfoBar::update()
{
	ControlContainer::update();
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	draw(renderer);
}


/**
 * Draws the resource information bar.
 */
void ResourceInfoBar::draw(NAS2D::Renderer& renderer) const
{
	renderer.drawBoxFilled(mRect.inset(1), NAS2D::Color{39, 39, 39});
	renderer.drawBox(mRect, NAS2D::Color{21, 21, 21});
	renderer.drawLine(NAS2D::Point{1, 0}, NAS2D::Point{renderer.size().x - 1, 0}, NAS2D::Color{56, 56, 56});

	// Resources
	int x = constants::MarginTight + 12;
	int offsetX = constants::ResourceIconSize + 40;
	auto position = NAS2D::Point{x, constants::MarginTight};
	constexpr auto textOffset = NAS2D::Vector{constants::ResourceIconSize + constants::Margin, 3 - constants::MarginTight};

	const auto drawIcon = [&renderer, &icons = mUiIcons, height = mRect.size.y](NAS2D::Point<int> iconPosition, NAS2D::Rectangle<int> imageRect)
	{
		const auto imageOffset = NAS2D::Vector{0, (height - imageRect.size.y - 1) / 2};
		renderer.drawSubImage(icons, iconPosition + imageOffset, imageRect);
	};

	const auto unpinnedImageRect = NAS2D::Rectangle<int>{{0, 72}, {8, 8}};
	const auto pinnedImageRect = NAS2D::Rectangle<int>{{8, 72}, {8, 8}};

	drawIcon(NAS2D::Point{2, 2}, mPinResourcePanel ? unpinnedImageRect : pinnedImageRect);
	drawIcon(NAS2D::Point{675, 2}, mPinPopulationPanel ? unpinnedImageRect : pinnedImageRect);

	const auto glowIntensity = calcGlowIntensity();
	const auto glowColor = NAS2D::Color{255, glowIntensity, glowIntensity};

	const std::array resources
	{
		std::tuple{ResourceImageRectsRefined[0], mResourcesCount.resources[0], offsetX},
		std::tuple{ResourceImageRectsRefined[1], mResourcesCount.resources[1], x + offsetX},
		std::tuple{ResourceImageRectsRefined[2], mResourcesCount.resources[2], x + offsetX},
		std::tuple{ResourceImageRectsRefined[3], mResourcesCount.resources[3], 0},
	};

	const auto& font = Control::getDefaultFont();

	for (const auto& [imageRect, amount, spacing] : resources)
	{
		drawIcon(position, imageRect);
		const auto color = (amount <= 10 && !mIgnoreGlow) ? glowColor : NAS2D::Color::White;
		renderer.drawText(font, std::to_string(amount), position + textOffset, color);
		position.x += spacing;
	}

	// Capacity (Storage, Food, Energy)
	const auto maxOreComponent = std::ranges::max(mResourcesCount.resources);
	const auto refinedOreCapacity = mStructureManager.totalRefinedOreStorageCapacity();
	const auto energyAvailable = mStructureManager.totalEnergyAvailable();
	const std::array storageCapacities
	{
		std::tuple{oreIconRect, maxOreComponent, refinedOreCapacity, false},
		std::tuple{foodIconRect, mFood, mStructureManager.totalFoodStorageCapacity(), mFood <= 10},
		std::tuple{powerIconRect, energyAvailable, mStructureManager.totalEnergyProduction(), energyAvailable <= 5}
	};

	position.x += x + offsetX;
	for (const auto& [imageRect, parts, total, isHighlighted] : storageCapacities)
	{
		drawIcon(position, imageRect);
		const auto color = (isHighlighted  && !mIgnoreGlow) ? glowColor : NAS2D::Color::White;
		const auto text = std::to_string(parts) + " / " + std::to_string(total);
		renderer.drawText(font, text, position + textOffset, color);
		position.x += (x + offsetX) * 2;
	}

	// Population / Morale
	position.x -= 13;
	int popMoraleDeltaImageOffsetX = mMorale.currentMorale() < mMorale.previousMorale() ? 0 : (mMorale.currentMorale() > mMorale.previousMorale() ? 8 : 16);
	const auto popMoraleDirectionImageRect = NAS2D::Rectangle<int>{{popMoraleDeltaImageOffsetX, 64}, {8, 8}};
	drawIcon(position, popMoraleDirectionImageRect);

	position.x += 13;
	const auto moraleLevel = (std::clamp(mMorale.currentMorale(), 1, 999) / 200);
	const auto popMoraleImageRect = NAS2D::Rectangle<int>{{176 + moraleLevel * constants::ResourceIconSize, 0}, {constants::ResourceIconSize, constants::ResourceIconSize}};
	drawIcon(position, popMoraleImageRect);
	renderer.drawText(font, std::to_string(mPopulationModel.getPopulations().size()), position + textOffset, NAS2D::Color::White);
}


void ResourceInfoBar::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> /*position*/)
{
	if (button == NAS2D::MouseButton::Left)
	{
		if (resourcePanelPinRect.contains(MOUSE_COORDS)) { mPinResourcePanel = !mPinResourcePanel; }
		if (populationPanelPinRect.contains(MOUSE_COORDS)) { mPinPopulationPanel = !mPinPopulationPanel; }
	}
}
