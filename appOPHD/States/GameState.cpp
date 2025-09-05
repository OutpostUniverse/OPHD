#include "GameState.h"

#include "MainMenuState.h"
#include "Wrapper.h"
#include "../StructureManager.h"
#include "../UI/MessageBox.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"

#include <NAS2D/Filesystem.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Mixer/Mixer.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Xml/XmlDocument.h>
#include <NAS2D/Xml/XmlMemoryBuffer.h>

#include <stdexcept>


NAS2D::Point<int> MOUSE_COORDS; /**< Mouse Coordinates. Used by other states/wrappers. */


GameState::GameState(const std::string& savedGameFilename) :
	mStructureManager{NAS2D::Utility<StructureManager>::get()},
	mSaveGameFile{savedGameFilename},
	mReportsState{{this, &GameState::onTakeMeThere}, {this, &GameState::onShowReports}, {this, &GameState::onHideReports}},
	mMapViewState{*this, mSaveGameFile, {this, &GameState::onQuit}},
	mColonyShip{colonyShipFromSave(mSaveGameFile.savedGameDocument())},
	mFileIoDialog{{this, &GameState::onLoadGame}, {this, &GameState::onSaveGame}}
{}


GameState::GameState(const PlanetAttributes& planetAttributes, Difficulty selectedDifficulty) :
	mStructureManager{NAS2D::Utility<StructureManager>::get()},
	mReportsState{{this, &GameState::onTakeMeThere}, {this, &GameState::onShowReports}, {this, &GameState::onHideReports}},
	mMapViewState{*this, planetAttributes, selectedDifficulty, {this, &GameState::onQuit}},
	mColonyShip{},
	mFileIoDialog{{this, &GameState::onLoadGame}, {this, &GameState::onSaveGame}}
{}


GameState::~GameState()
{
	mStructureManager.removeAllStructures();

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseMotion().disconnect({this, &GameState::onMouseMove});

	NAS2D::Utility<NAS2D::Mixer>::get().removeMusicCompleteHandler({this, &GameState::onMusicComplete});
	NAS2D::Utility<NAS2D::Mixer>::get().stopAllAudio();
}


void GameState::initializeGameState()
{
	mReportsState.initialize();

	mMapViewState.initialize();
	initializeMapViewState();

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseMotion().connect({this, &GameState::onMouseMove});

	NAS2D::Utility<NAS2D::Mixer>::get().addMusicCompleteHandler({this, &GameState::onMusicComplete});
}


void GameState::initializeMapViewState()
{
	mActiveState = &mMapViewState;
	mMapViewState.activate();

	mMapViewState.mapChangedHandler({this, &GameState::onMapChange});
}


void GameState::initialize()
{
	mFade.fadeIn(constants::FadeSpeed);
	initializeGameState();
}


void GameState::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	MOUSE_COORDS = position;
}


/**
 * Music Complete event handler.
 *
 * Called by NAS2D::Mixer upon completion of a music track. This function
 * changes the background music track to a different track in the lineup.
 */
void GameState::onMusicComplete()
{
	/// \todo	Make me work... once there's some music to listen to. 0.0
}


/**
 * Event handler that responds to a quit event raised by the MapViewState.
 *
 * This event is raised on game overs and when the user chooses the "Return
 * to Main Menu" from the system options window.
 */
void GameState::onQuit()
{
	mMapViewState.deactivate();
	mReportsState.deactivate();
}


/**
 * Event handler that responds to a show reports event raised by the MapViewState.
 *
 * This event is raised whenever a user double-clicks on a factory in the MapViewState.
 */
void GameState::onShowReports()
{
	mActiveState->deactivate();
	mActiveState = &mReportsState;
	mActiveState->activate();
}


/**
 * Event handler that responds to a hide report event raised by the ReportsState.
 *
 * This event is raised by the ReportsState whenever the user clicks the Exit
 * UI panel or if the Escape key is pressed.
 */
void GameState::onHideReports()
{
	mActiveState->deactivate();
	mActiveState = &mMapViewState;
	mActiveState->activate();
}


void GameState::onMapChange()
{
	mReportsState.clearLists();
}


void GameState::onLoadGame(const std::string& saveGameName)
{
	auto& filesystem = NAS2D::Utility<NAS2D::Filesystem>::get();
	auto saveGamePath = constants::SaveGamePath + saveGameName + ".xml";
	try
	{
		if (!filesystem.exists(saveGamePath)) { throw std::runtime_error("Save game file does not exist: " + saveGamePath); }
	}
	catch (const std::exception& e)
	{
		doNonFatalErrorMessage("Load Failed", e.what());
		return;
	}

	mReturnState = new GameState{saveGamePath};
}


void GameState::onSaveGame(const std::string& saveGameName)
{
	mFileIoDialog.hide();
	auto saveGamePath = constants::SaveGamePath + saveGameName + ".xml";
	SavedGameFile savedGameFile;
	mMapViewState.save(savedGameFile);

	// Write out the XML file.
	NAS2D::Xml::XmlMemoryBuffer buff;
	savedGameFile.savedGameDocument().accept(&buff);

	NAS2D::Utility<NAS2D::Filesystem>::get().writeFile(saveGamePath, buff.buffer());
}


/**
 * Event handler that responds to a 'take me there' event raised by the ReportsState.
 *
 * This event is raised by the ReportsState whenever a "Take Me There" button in any
 * of the report UI panels is clicked.
 */
void GameState::onTakeMeThere(const Structure* structure)
{
	onHideReports();
	mMapViewState.focusOnStructure(structure);
}


NAS2D::State* GameState::update()
{
	if (mActiveState)
	{
		mActiveState->update();
	}

	mFade.update();
	mFade.draw(NAS2D::Utility<NAS2D::Renderer>::get());

	if (mMapViewState.hasGameEnded())
	{
		mReturnState = new MainMenuState();
	}

	return mReturnState;
}
