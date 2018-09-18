#pragma once

#include "Wrapper.h"

#include "../UI/Core/ComboBox.h"

#include "../Things/Structures/Structure.h"

class MainReportsUiState : public Wrapper
{
public:
	MainReportsUiState();
	virtual ~MainReportsUiState();

	void addFactories(StructureList&);

protected:
	void initialize();
	State* update();

private:
	virtual void _deactivate() final;
	virtual void _activate() final;

private:
	void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	void onMouseMotion(int x, int y, int dx, int dy);
	void onWindowResized(int w, int h);

private:
	NAS2D::State*	mReturnState = this;
};
