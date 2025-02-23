#pragma once

#include "Planet.h"
#include "../UI/FileIo.h"

#include <NAS2D/State.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/Fade.h>

#include <string>
#include <memory>


enum class Difficulty;

class MainReportsUiState;
class MapViewState;
class Structure;
class Wrapper;


class GameState : public NAS2D::State
{
public:
	GameState(const std::string& savedGameFilename);
	GameState(const Planet::Attributes& planetAttributes, Difficulty selectedDifficulty);
	~GameState() override;

	State* update() override;

protected:
	GameState();
	void initializeMapViewState(MapViewState*);
	void initialize() override;

	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

	void onFadeComplete();
	void onMusicComplete();

	void onQuit();
	void onShowReports();
	void onHideReports();
	void onMapChange();
	void onLoadGame(const std::string&);

	void onTakeMeThere(const Structure*);

private:
	std::unique_ptr<MainReportsUiState> mMainReportsState;
	std::unique_ptr<MapViewState> mMapView;
	std::unique_ptr<MapViewState> mNewMapView;
	Wrapper* mActiveState = nullptr;
	NAS2D::State* mReturnState = this;
	NAS2D::Fade mFade;
};
