#pragma once

#include "UI.h"

class MainMenuOptions : public Window
{
public:
	MainMenuOptions();
	virtual ~MainMenuOptions() override;

	virtual void update() override;

protected:
	virtual void init();
	virtual void visibilityChanged(bool visible) override;

private:
	void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);
	void onSoundVolumeChanged(float newValue);
	void onMusicVolumeChanged(float newValue);
	void onOptionsChanged();
	void onVideoOptionsChanged();

	void setConfigFromControls();
	void setControlsFromConfig();

	void enableControls();
	void disableControls();

	void enableButtons();
	void disableButtons();

	void btnOkClicked();
	void btnResolutionOkClicked();
	void btnCancelClicked();
	void btnApplyClicked();

	void saveOptions();
	void loadOptions();
	void applyVideoChanges();

	UIContainer pnlControls;

	Label lblResolution;
	Label lblFullscreen;
	Label lblVSync;
	Label lblMusicVolume;
	Label lblSoundVolume;
	Label lblSkipSplash;
	Label lblStartMaximized;

	ComboBox cmbResolution;
	CheckBox cbxFullscreen;
	CheckBox cbxVSync;
	CheckBox cbxSkipSplash;
	CheckBox cbxStartMaximized;
	Slider sldrMusicVolume;
	Slider sldrSoundVolume;

	UIContainer pnlButtons;
	Button btnOk;
	Button btnCancel;
	Button btnApply;

	Window dlgResolutionChanged;
	Button btnResolutionChangedOk;
	Label lblResolutionChanged;

	bool optionsChanged = false;
	bool videoOptionsChanged = false;
	bool inInit = true;
};
