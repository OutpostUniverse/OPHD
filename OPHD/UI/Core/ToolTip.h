#pragma once

#include "Control.h"

#include <NAS2D/Math/Point.h>
#include <NAS2D/Resource/Font.h>
#include <NAS2D/Timer.h>

#include <utility>
#include <vector>


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

	std::pair<Control*, std::string>* mFocusedControl{nullptr};

	std::vector<std::pair<Control*, std::string>> mControls;
};
