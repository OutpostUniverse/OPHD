// ==================================================================================
// = NAS2D+GUI
// = Copyright © 2008 - 2012, New Age Software
// ==================================================================================
// = This file is part of the NAS2D+GUI library.
// ==================================================================================

#pragma once

#include "NAS2D/EventHandler.h"
#include "NAS2D/Timer.h"
#include "NAS2D/Resources/Image.h"
#include "Control.h"

#include "NAS2D/Renderer/Color.h"
#include "NAS2D/Resources/Font.h"

/**
 * \class Label
 * \brief A control that contains readonly text.
 * 
 */

class Label : public Control
{
public:
	Label();
	virtual ~Label();

	void font(NAS2D::Font* font);
	bool empty() const { return text().empty(); }
	void clear() { _text().clear(); }
	void update();

	void color(const NAS2D::Color& color);

protected:
private:
	NAS2D::Color textColor{NAS2D::Color::White};
};
