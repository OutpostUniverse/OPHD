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

	void add(const Control& control, const std::string& toolTipText);

	void update() override;

protected:
	struct ControlText
	{
		const Control& control;
		std::string text;
	};

	void draw(NAS2D::Renderer& renderer) const override;

	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

	void setToolTipArea(ControlText& controlText, int mouseX);

private:
	const NAS2D::Font& mFont;
	NAS2D::Timer mTimer;

	std::vector<ControlText> mControlTexts;
	ControlText* mFocus;
};
