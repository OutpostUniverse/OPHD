#pragma once

#include "Control.h"

#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Timer.h>

#include <string>
#include <vector>


namespace NAS2D
{
	class Font;
}


class ToolTip : public Control
{
public:
	ToolTip();
	~ToolTip() override;

	void add(Control&, const std::string&);

	void update() override;

protected:
	struct ControlText
	{
		Control* control;
		std::string text;
	};

	void draw() const override;

	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

	void buildDrawParams(ControlText&, int);

private:
	const NAS2D::Font& mFont;
	NAS2D::Timer mTimer;

	ControlText* mFocusedControl{nullptr};

	std::vector<ControlText> mControls;
};
