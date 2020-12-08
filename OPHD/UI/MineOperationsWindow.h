#pragma once

#include "Core/Window.h"
#include "Core/Button.h"
#include "Core/CheckBox.h"

#include <NAS2D/Renderer/RectangleSkin.h>


class MineFacility;


/**
 * \brief Implements the Mine Facility Operations Window
 */
class MineOperationsWindow final : public Window
{
public:
	MineOperationsWindow();

	void mineFacility(MineFacility* facility);
	MineFacility* mineFacility() { return mFacility; }

	void updateTruckAvailability();

	void update() override;
	void hide() override;

private:
	MineOperationsWindow(const MineOperationsWindow&) = delete;
	MineOperationsWindow& operator=(const MineOperationsWindow&) = delete;

private:
	void chkCommonMetalsClicked();
	void chkCommonMineralsClicked();
	void chkRareMetalsClicked();
	void chkRareMineralsClicked();

	void btnOkayClicked();
	void btnExtendShaftClicked();
	void btnIdleClicked();

	void btnAssignTruckClicked();
	void btnUnassignTruckClicked();

private:
	MineFacility* mFacility = nullptr;

	const NAS2D::Image& mUiIcon;
	const NAS2D::Image& mIcons;

	NAS2D::RectangleSkin mPanel;

	CheckBox chkCommonMetals;
	CheckBox chkCommonMinerals;
	CheckBox chkRareMetals;
	CheckBox chkRareMinerals;

	Button btnIdle;
	Button btnExtendShaft;
	Button btnOkay;

	Button btnAssignTruck;
	Button btnUnassignTruck;

	int mAvailableTrucks = 0;
};
