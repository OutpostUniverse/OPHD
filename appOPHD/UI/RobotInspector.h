#pragma once

#include <libControls/Button.h>
#include <libControls/Window.h>


class Robot;


class RobotInspector : public Window
{
public:
	RobotInspector();

	void focusOnRobot(Robot* robot);
	const Robot* focusedRobot() const { return mRobot; }

	void drawClientArea(NAS2D::Renderer& renderer) const override;

private:
	void onCancelOrders();
	void onSelfDestruct();
	void onCancel();

private:
	Button btnCancelOrders;
	Button btnSelfDestruct;
	Button btnCancel;

	NAS2D::Rectangle<int> mContentRect;

	Robot* mRobot{nullptr};
};
