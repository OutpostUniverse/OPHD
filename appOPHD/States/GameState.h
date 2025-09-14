#pragma once

#include "MapViewState.h"
#include "ReportsState.h"
#include "ColonyShip.h"

#include "../SavedGameFile.h"
#include "../UI/FileIo.h"

#include <libOPHD/PlanetAttributes.h>

#include <NAS2D/State.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/Fade.h>

#include <string>


namespace NAS2D
{
	enum class KeyCode : uint32_t;
	enum class KeyModifier : uint16_t;
}

enum class Difficulty;

class StructureManager;
class Structure;
class Wrapper;


class GameState : public NAS2D::State
{
public:
	GameState(const std::string& savedGameFilename);
	GameState(const PlanetAttributes& planetAttributes, Difficulty selectedDifficulty);
	~GameState() override;

	State* update() override;

	ReportsState& reportsState() { return mReportsState; }
	FileIo& fileIoDialog() { return mFileIoDialog; }

	StructureManager& structureManager() { return mStructureManager; }
	ColonyShip& colonyShip() { return mColonyShip; }

protected:
	void initializeGameState();
	void initializeMapViewState();
	void initialize() override;

	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);
	void onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier mod, bool /*repeat*/);

	void onFadeComplete();
	void onMusicComplete();

	void onQuit();
	void onShowReports();
	void onHideReports();
	void onMapChange();
	void onLoadGame(const std::string&);
	void onSaveGame(const std::string&);

	void onTakeMeThere(const Structure*);

private:
	StructureManager& mStructureManager;
	SavedGameFile mSaveGameFile;
	ReportsState mReportsState;
	MapViewState mMapViewState;
	ColonyShip mColonyShip;
	Wrapper* mActiveState = nullptr;
	NAS2D::State* mReturnState = this;
	NAS2D::Fade mFade;
	FileIo mFileIoDialog;
};
