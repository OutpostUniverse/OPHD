#pragma once

#include <NAS2D/NAS2D.h>

class MapViewState;
class Structure;

class GameState : public NAS2D::State
{
public:
	GameState();
	virtual ~GameState();

	void mapviewstate(MapViewState*);

	virtual void initialize() final;
	virtual State* update() final;

private:
	void onMouseMotion(int x, int y, int relX, int relY);

	void fadeComplete();
	void musicComplete();

	void quitEvent();
	void showReportsUi();
	void hideReportsUi();

	void takeMeThere(Structure*);

private:
	NAS2D::State*	mReturnState = this;
};
