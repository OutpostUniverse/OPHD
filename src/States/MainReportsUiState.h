#pragma once

#include "Wrapper.h"

class MainReportsUiState : public Wrapper
{
public:
	MainReportsUiState();
	virtual ~MainReportsUiState();

protected:
	void initialize();
	State* update();

	void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);

	
private:
	virtual void _deactivate() final;
	virtual void _activate() final;

private:

	NAS2D::State*	mReturnState = this;
};
