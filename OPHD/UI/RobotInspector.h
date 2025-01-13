#pragma once

#include <libControls/Button.h>
#include <libControls/Window.h>

#include "../MapObjects/Robot.h"


class RobotInspector : public Window
{
public:
	RobotInspector();

	void focusOnRobot(Robot*);
	const Robot* focusedRobot() const { return mRobot; }

	NAS2D::Signal<Robot*>& actionButtonClicked() { return mSignal; }

	void update() override;

private:
	void onCancelOrders();
	void onSelfDestruct();
	void onCancel();

private:
	Button btnCancelOrders;
	Button btnSelfDestruct;
	Button btnCancel;

	NAS2D::Rectangle<int> mContentRect;

	NAS2D::Signal<Robot*> mSignal;

	Robot* mRobot{nullptr};
};
