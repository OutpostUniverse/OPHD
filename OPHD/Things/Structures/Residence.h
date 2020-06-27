#pragma once

#include "Structure.h"

#include "../../FontManager.h"
#include "../../Constants.h"
#include <NAS2D/Resources/Font.h>
#include <NAS2D/Utility.h>


/**
 * \class	Residence
 * \brief	Base Residential structure.
 * 
 * Implements the base Residence structures. Upgraded residences should derive
 * from this class and override the definition of mCapacity.
 */
class Residence : public Structure
{
public:
	Residence() : Structure(constants::RESIDENCE, "structures/residential_1.sprite", StructureClass::CLASS_RESIDENCE)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(2);

		requiresCHAP(true);
	}

	int capacity() const { return mCapacity; }

	void drawInspectorView(NAS2D::Renderer& renderer, const NAS2D::Rectangle<float>& windowRect) override
	{
		const auto drawLabelAndValue = [&renderer](NAS2D::Point<int> position, const std::string& title, const std::string& text) {
			NAS2D::Font* FONT = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
			NAS2D::Font* FONT_BOLD = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);

			renderer.drawText(*FONT_BOLD, title, position, NAS2D::Color::White);
			position.x() += FONT_BOLD->width(title);
			renderer.drawText(*FONT, text, position, NAS2D::Color::White);
		};

		auto position = windowRect.startPoint() + NAS2D::Vector{ 10, 135 };
		drawLabelAndValue(position, "Colonist Capacity: ", std::to_string(capacity()));
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn().energy(2);
	}

protected:
	int mCapacity = 25; /**< Override this value in derived residences.*/
};
