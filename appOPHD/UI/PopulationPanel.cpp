#include "PopulationPanel.h"

#include "TrendLabel.h"

#include "../CacheImage.h"
#include "../MoraleString.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"

#include <libOPHD/EnumMoraleIndex.h>
#include <libOPHD/Population/PopulationModel.h>
#include <libOPHD/Population/PopulationPool.h>
#include <libOPHD/Population/Morale.h>
#include <libOPHD/Population/MoraleChangeEntry.h>

#include <libControls/LoadRectangleSkin.h>

#include <NAS2D/Resource/Font.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>
#include <array>


namespace
{
	static constexpr int IconSize = 32;


	static std::size_t moraleIndex(int morale)
	{
		return static_cast<std::size_t>(std::clamp(morale, 0, 999) / 200);
	}


	static const std::array moraleStringColor
	{
		NAS2D::Color::Red,
		NAS2D::Color::Orange,
		NAS2D::Color::Yellow,
		NAS2D::Color::White,
		constants::PrimaryColor
	};
}


PopulationPanel::PopulationPanel(const PopulationModel& pop, const PopulationPool& popPool, const Morale& morale) :
	mFont{Control::getDefaultFont()},
	mFontBold{Control::getDefaultFontBold()},
	mIcons{imageCache.load("ui/icons.png")},
	mSkin{loadRectangleSkin("ui/skin/window")},
	mPopulationModel(pop),
	mPopulationPool(popPool),
	mMorale(morale)
{
	constexpr int linesOfText = 16;
	constexpr int edgeBuffer = constants::Margin * 2;
	const int windowHeight = mFontBold.height() + (mFont.height() * linesOfText) + (edgeBuffer * 2 /* Times two to account for both the edge and the divider line. */);

	int largestStringLength = mFontBold.width(constants::MoraleBreakdown);
	for (std::size_t i = 0; i < moraleStringTableCount(); ++i)
	{
		const int lengthCompare = mFont.width(moraleString(i));
		if (lengthCompare > largestStringLength)
		{
			largestStringLength = lengthCompare;
		}
	}

	largestStringLength += edgeBuffer + mFont.width("999999");

	mPopulationPanelWidth = mFontBold.width(constants::PopulationBreakdown) + edgeBuffer;
	const int windowWidth = largestStringLength + edgeBuffer + mPopulationPanelWidth;


	size({windowWidth, windowHeight});
}


PopulationPanel::~PopulationPanel()
{
}


void PopulationPanel::draw(NAS2D::Renderer& renderer) const
{
	mSkin.draw(renderer, mRect);

	const int fontHeight = mFont.height();
	const int fontBoldHeight = mFontBold.height();

	auto position = this->position() + NAS2D::Vector{constants::Margin, constants::Margin};

	// POPULATION Statistics
	renderer.drawText(mFontBold, constants::PopulationBreakdown, position);
	const auto& population = mPopulationModel.getPopulations();
	const std::array populationData
	{
		std::tuple{NAS2D::Rectangle<int>{{0, 96}, {IconSize, IconSize}}, population.child, std::string("Children")},
		std::tuple{NAS2D::Rectangle<int>{{32, 96}, {IconSize, IconSize}}, population.student, std::string("Students")},
		std::tuple{NAS2D::Rectangle<int>{{64, 96}, {IconSize, IconSize}}, population.worker, std::string("Workers")},
		std::tuple{NAS2D::Rectangle<int>{{96, 96}, {IconSize, IconSize}}, population.scientist, std::string("Scientists")},
		std::tuple{NAS2D::Rectangle<int>{{128, 96}, {IconSize, IconSize}}, population.retiree, std::string("Retired")},
	};

	position.y += fontBoldHeight + constants::Margin;
	const auto textOffset = NAS2D::Vector{IconSize + constants::Margin, (IconSize / 2) - (fontHeight / 2)};

	for (const auto& [imageRect, personCount, personRole] : populationData)
	{
		renderer.drawSubImage(mIcons, position, imageRect);

		renderer.drawText(mFont, personRole + ": ", position + textOffset);

		const auto personCountString = std::to_string(personCount);
		const NAS2D::Point<int> labelPosition = {this->position().x + mPopulationPanelWidth - mFont.width(personCountString) - constants::Margin, position.y + textOffset.y};
		renderer.drawText(mFont, personCountString, labelPosition);

		position.y += IconSize + constants::Margin;
	}

	// DIVIDER LINE Between population statistics and population availability statistics
	renderer.drawLine(position, position + NAS2D::Vector<int>{mFont.width(constants::PopulationBreakdown) + constants::Margin, 0}, NAS2D::Color::DarkGray);
	position.y += constants::Margin;

	const std::array populationAvailabilityStatistics{
		std::tuple{"Available Workers: ", mPopulationPool.availableWorkers()},
		std::tuple{"Available Scientists: ", mPopulationPool.availableScientists()},
	};

	for (const auto& [statisticLabel, personCount] : populationAvailabilityStatistics)
	{
		const auto personCountString = std::to_string(personCount);
		const NAS2D::Color statusColor = personCount <= 0 ? NAS2D::Color::Red : NAS2D::Color::White;
		renderer.drawText(mFont, statisticLabel, position, NAS2D::Color::White);
		const NAS2D::Point<int> labelPosition = {this->position().x + mPopulationPanelWidth - mFont.width(personCountString) - constants::Margin, position.y};
		renderer.drawText(mFont, personCountString, labelPosition, statusColor);
		position.y += fontBoldHeight + constants::Margin;
	}

	// DIVIDER LINE Between population statistics and morale statistics
	position = this->position() + NAS2D::Vector{mPopulationPanelWidth, constants::Margin};
	renderer.drawLine(position, position + NAS2D::Vector<int>{0, area().size.y - 10}, NAS2D::Color::DarkGray);

	// MORALE
	position.x += constants::Margin;
	renderer.drawText(mFontBold, constants::MoraleBreakdown, position);

	position.y += fontBoldHeight;
	const auto moraleLevel = moraleIndex(mMorale.currentMorale());
	renderer.drawText(mFont, moraleString(MoraleIndexs::Description) + moraleString(moraleLevel), position, moraleStringColor[moraleLevel]);

	position.y += fontHeight;
	renderer.drawText(mFont, "Current: " + std::to_string(mMorale.currentMorale()) + " / Previous: " + std::to_string(mMorale.previousMorale()), position);

	position.y += fontHeight;
	int capacityPercent = (mResidentialCapacity > 0) ? (population.size() * 100 / mResidentialCapacity) : 0;
	const auto housingText = "Housing: " + std::to_string(population.size()) + " / " + std::to_string(mResidentialCapacity) + "  (" + std::to_string(capacityPercent) + "%)";
	renderer.drawText(mFont, housingText, position, NAS2D::Color::White);

	position.y += fontHeight;
	renderer.drawText(mFont, "Mean Crime Rate: " + std::to_string(mCrimeRate) + "%", position, NAS2D::Color::White);

	position.y += fontHeight + constants::Margin;

	// DIVIDER LINE Between morale breakdown and morale change reasons
	renderer.drawLine(position, position + NAS2D::Vector<int>{area().size.x - mPopulationPanelWidth - constants::Margin * 2, 0}, NAS2D::Color::DarkGray);

	position.y += constants::Margin;

	for (auto& entry : mMorale.moraleChangeJournal())
	{
		if (entry.value == 0) { continue; }
		renderer.drawText(mFont, entry.description , position);

		const auto text = formatDiff(entry.value);
		const NAS2D::Point<int> labelPosition = {area().position.x + area().size.x - mFont.width(text) - 5 , position.y};

		drawTrendLabel(renderer, mFont, entry.value, labelPosition);
		position.y += fontHeight;
	}
}
