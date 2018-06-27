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

	virtual void update() final;

	virtual void hide() final;

protected:
	virtual void init() final;

private:
	void btnOkayClicked();
	void btnCancelClicked();
	void btnExtendShaftClicked();


	MineOperationsWindow() = delete;
	MineOperationsWindow(const MineOperationsWindow&) = delete;
	MineOperationsWindow& operator=(const MineOperationsWindow&) = delete;


private:
	MineFacility*		mFacility = nullptr;

	Font				mBold;

	Image				mUiIcon;

	Button				btnOkay;
	Button				btnCancel;

	Button				btnExtendShaft;
};
