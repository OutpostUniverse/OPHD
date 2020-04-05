#pragma once

#include "UI.h"

#include "../Mine.h"
#include "../Things/Structures/MineFacility.h"


/**
 * \brief Implements the Mine Facility Operations Window
 */
class MineOperationsWindow final : public Window
{
public:
	MineOperationsWindow();
	~MineOperationsWindow() final;

	void mineFacility(MineFacility* facility);
	MineFacility* mineFacility() { return mFacility; }
	
	void updateCounts();

	void update() override final;
	void hide() override final;

protected:
	virtual void init() final;

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

private:
	MineFacility*		mFacility = nullptr;

	NAS2D::Image		mUiIcon;
	NAS2D::Image		mIcons;

	NAS2D::ImageList	mPanel;

	CheckBox			chkCommonMetals;
	CheckBox			chkCommonMinerals;
	CheckBox			chkRareMetals;
	CheckBox			chkRareMinerals;

	Button				btnIdle;
	Button				btnExtendShaft;
	Button				btnOkay;
};
