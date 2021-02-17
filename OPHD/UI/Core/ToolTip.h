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
	~ToolTip() override;

	void add(Control&, const std::string&);
	void update() override;

private:
	void onMouseMove(int, int, int, int);

	void buildDrawParams(std::pair<Control*, std::string>&, int);

private:
	const NAS2D::Font& mFont;
	NAS2D::Timer mTimer;

	std::pair<Control*, std::string>* mFocusedControl{ nullptr };

	std::vector<std::pair<Control*, std::string>> mControls;
};
