#pragma once

#include <NAS2D/State.h>

class MapViewState;
class Structure;

class GameState : public NAS2D::State
{
public:
	GameState();
	~GameState() override;

	void mapviewstate(MapViewState*);

	void initialize() override;
	State* update() override;

private:
	void onMouseMove(int x, int y, int relX, int relY);

	void fadeComplete();
	void musicComplete();

	void quitEvent();
	void showReportsUi();
	void hideReportsUi();
	void mapChanged();

	void takeMeThere(Structure*);

private:
	NAS2D::State*	mReturnState = this;
};
