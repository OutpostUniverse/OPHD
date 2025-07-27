#include "ResourceInfoBar.h"

#include "../Cache.h"
#include "../Resources.h"
#include "../Constants/Numbers.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../States/MapViewStateHelper.h"
#include "../MapObjects/StructureClass.h"
#include "../MapObjects/Structure.h"

#include "../StructureManager.h"

#include <libOPHD/Population/Population.h>
#include <libOPHD/Population/Morale.h>

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Timer.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Renderer/Renderer.h>

#include <cstdint>
#include <algorithm>


extern NAS2D::Point<int> MOUSE_COORDS;


namespace
{
	constexpr NAS2D::Rectangle<int> ResourcePanelPinRect{{0, 1}, {8, 19}};
	constexpr NAS2D::Rectangle<int> PopulationPanelPinRect{{675, 1}, {8, 19}};


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


ResourceInfoBar::ResourceInfoBar(const StorableResources& resources, const Population& population, const Morale& morale, const int& food) :
	ControlContainer{{&mToolTip}},
	mResourcesCount{resources},
	mPopulation{population},
	mMorale{morale},
	mFood{food},
	mUiIcons{imageCache.load("ui/icons.png")}
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
	renderer.drawBoxFilled(mRect, NAS2D::Color{39, 39, 39});
	renderer.drawBox(mRect, NAS2D::Color{21, 21, 21});
	renderer.drawLine(NAS2D::Point{1, 0}, NAS2D::Point{renderer.size().x - 2, 0}, NAS2D::Color{56, 56, 56});

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

	constexpr auto iconSize = constants::ResourceIconSize;
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
	const auto& sm = NAS2D::Utility<StructureManager>::get();
	const std::array storageCapacities
	{
		std::tuple{NAS2D::Rectangle<int>{{96, 32}, {iconSize, iconSize}}, mResourcesCount.total(), totalStorage(StructureClass::Storage, 1000), totalStorage(StructureClass::Storage, 1000) - mResourcesCount.total() <= 100},
		std::tuple{NAS2D::Rectangle<int>{{64, 32}, {iconSize, iconSize}}, mFood, sm.totalFoodStorageCapacity(), mFood <= 10},
		std::tuple{NAS2D::Rectangle<int>{{80, 32}, {iconSize, iconSize}}, sm.totalEnergyAvailable(), sm.totalEnergyProduction(), sm.totalEnergyAvailable() <= 5}
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
	renderer.drawText(font, std::to_string(mPopulation.getPopulations().size()), position + textOffset, NAS2D::Color::White);
}


void ResourceInfoBar::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> /*position*/)
{
	if (button == NAS2D::MouseButton::Left)
	{
		if (ResourcePanelPinRect.contains(MOUSE_COORDS)) { mPinResourcePanel = !mPinResourcePanel; }
		if (PopulationPanelPinRect.contains(MOUSE_COORDS)) { mPinPopulationPanel = !mPinPopulationPanel; }
	}
}


/**
 * Get the total amount of storage given a structure class and capacity of each
 * structure.
 */
int ResourceInfoBar::totalStorage(StructureClass structureClass, int capacity) const
{
	int storageCapacity = 0;

	// Command Center has a limited amount of storage for when colonists first land.
	if (isCcPlaced())
	{
		storageCapacity += constants::BaseStorageCapacity;
	}

	const auto& structures = NAS2D::Utility<StructureManager>::get().structureList(structureClass);
	for (const auto* structure : structures)
	{
		if (structure->operational() || structure->isIdle())
		{
			storageCapacity += capacity;
		}
	}

	return storageCapacity;
}
