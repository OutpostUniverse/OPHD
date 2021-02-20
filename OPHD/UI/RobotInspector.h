#pragma once

#include "Core/Button.h"
#include "Core/TextArea.h"
#include "Core/Window.h"

#include "StringTable.h"

#include "../Common.h"

#include "../Things/Robots/Robot.h"


class RobotInspector : public Window
{
public:
	RobotInspector();

	void focusOnRobot(Robot*);

	NAS2D::Signals::Signal<Robot*>& actionButtonClicked() { return mCallback; }

	void update() override;

private:
	void init();

private:
	Button btnCancelOrders;
	Button btnSelfDestruct;
	Button btnCancel;

	NAS2D::Rectangle<int> mContentArea;

	NAS2D::Signals::Signal<Robot*> mCallback;

	Robot* mRobot{ nullptr };
};
