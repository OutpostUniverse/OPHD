#pragma once

#include "Control.h"

#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
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

protected:
	void draw() const override;

private:
	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

	void buildDrawParams(std::pair<Control*, std::string>&, int);

private:
	const NAS2D::Font& mFont;
	NAS2D::Timer mTimer;

	std::pair<Control*, std::string>* mFocusedControl{nullptr};

	std::vector<std::pair<Control*, std::string>> mControls;
};
