#pragma once

#include "../UI/FileIo.h"

#include <libControls/Label.h>

#include <NAS2D/State.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Renderer/Fade.h>

#include <array>
#include <cstdint>


namespace NAS2D
{
	enum class KeyModifier : uint16_t;
	enum class KeyCode : uint32_t;
}


/**
 * Implements the main menu screen.
 */
class MainMenuState : public NAS2D::State
{
public:
	MainMenuState();
	~MainMenuState() override;

protected:
	void initialize() override;
	State* update() override;

private:
	void onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier mod, bool repeat);
	void onWindowResized(NAS2D::Vector<int> newSize);
	void onFadeComplete();

	void positionButtons();
	void disableButtons();
	void enableButtons();

	void onNewGame();
	void onContinueGame();
	void onOptions();
	void onHelp();
	void onQuit();

	void onLoadGame(const std::string& filePath);

private:
	const NAS2D::Image mBgImage;
	std::array<Button, 4> buttons;
	Label lblVersion;
	NAS2D::State* mReturnState = this;
	NAS2D::Fade mFade;
	FileIo mFileIoDialog;
};
