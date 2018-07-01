#pragma once

#include "UI.h"

#include "../Mine.h"
#include "../Things/Structures/MineFacility.h"


/**
 * \brief Implements the Mine Facility Operations Window
 */
class MineOperationsWindow : public Window
{
public:
	MineOperationsWindow(Font& font);
	virtual ~MineOperationsWindow() final;

	void mineFacility(MineFacility* _mf);
	void updateCounts();

	virtual void update() final;
	virtual void hide() final;

protected:
	virtual void init() final;

private:
	MineOperationsWindow() = delete;
	MineOperationsWindow(const MineOperationsWindow&) = delete;
	MineOperationsWindow& operator=(const MineOperationsWindow&) = delete;

private:
	void btnCommonMetalsClicked();
	void btnCommonMineralsClicked();
	void btnRareMetalsClicked();
	void btnRareMineralsClicked();

	void btnOkayClicked();
	void btnExtendShaftClicked();
	void btnIdleClicked();

private:
	MineFacility*		mFacility = nullptr;

	Font				mBold;

	Image				mUiIcon;
	Image				mIcons;

	ImageList			mPanel;

	Button				btnCommonMetals;
	Button				btnCommonMinerals;
	Button				btnRareMetals;
	Button				btnRareMinerals;

	Button				btnIdle;
	Button				btnExtendShaft;
	Button				btnOkay;
};
