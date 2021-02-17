#pragma once

#include "Control.h"

#include <NAS2D/Renderer/Point.h>
#include <NAS2D/Resources/Font.h>
#include <NAS2D/Timer.h>

#include <utility>
#include <vector>


/**
 * \class Label
 * \brief A control that contains readonly text.
 *
 */
class ToolTip : public Control
{
public:
	ToolTip();
	virtual ~ToolTip();

	void add(Control&, const std::string&);
	void update() override;

private:
	void onMouseMove(int, int, int, int);

private:
	const NAS2D::Font& mFont;
	NAS2D::Timer mTimer;

	NAS2D::Point<int> mMouseCoords;

	std::pair<Control*, std::string>* mFocusedControl;

	std::vector<std::pair<Control*, std::string>> mControls;
};
