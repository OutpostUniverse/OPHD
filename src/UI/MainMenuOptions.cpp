// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MainMenuOptions.h"

#include "NAS2D/Renderer/Renderer.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Configuration.h"
#include "NAS2D/Mixer/Mixer.h"
#include "NAS2D/StringUtils.h"

#include "../Constants.h"

#include <sstream>
#include <cmath>

#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"


MainMenuOptions::MainMenuOptions() :
	Window(constants::WINDOW_SYSTEM_TITLE),
	lblResolution{"Resolution"},
	lblFullscreen{"Fullscreen"},
	lblVSync{"VSync"},
	lblMusicVolume{"Music Volume"},
	lblSoundVolume{"Sound Volume"},
	lblSkipSplash{"Quickstart"},
	lblStartMaximized{"Start Maximized"},
	btnOk{"Ok"},
	btnCancel{"Cancel"},
	btnApply{"Apply"},
	dlgResolutionChanged{"Notice"},
	btnResolutionChangedOk{"Ok"},
	lblResolutionChanged{"You must restart the game for video options to take effect."}
{
	init();
}


MainMenuOptions::~MainMenuOptions()
{
	auto& e = NAS2D::Utility<NAS2D::EventHandler>::get();
	e.keyDown().disconnect(this, &MainMenuOptions::onKeyDown);
	btnOk.click().disconnect(this, &MainMenuOptions::btnOkClicked);
	btnCancel.click().disconnect(this, &MainMenuOptions::btnCancelClicked);
	btnApply.click().disconnect(this, &MainMenuOptions::btnApplyClicked);
}


void MainMenuOptions::init()
{

	auto& e = NAS2D::Utility<NAS2D::EventHandler>::get();
	e.keyDown().connect(this, &MainMenuOptions::onKeyDown);

	size(500, 350);

	const auto border_left_width = 5;
	const auto border_right_width = 5;
	constexpr auto border_top_height = sWindowTitleBarHeight + 5;
	const auto border_bottom_height = 5;
	const auto element_spacing = 5;
	const auto left_edge = positionX() + border_left_width;
	const auto right_edge = left_edge + width() - border_right_width * 2.0f;
	const auto top_edge = positionY() + border_top_height;
	const auto bottom_edge = top_edge + height() - border_top_height - border_bottom_height * 2.0f;
	const auto width = right_edge - left_edge;
	const auto height = bottom_edge - top_edge;

	pnlControls.position(left_edge, top_edge);
	pnlControls.size(width, height);

	pnlButtons.position(pnlControls.positionX(), bottom_edge - element_spacing);
	pnlButtons.size(pnlControls.width(), 120);

	lblResolution.size(0, 0);

	cmbResolution.visible(true);
	cmbResolution.enabled(true);
	cmbResolution.size(200, 0);
	cmbResolution.maxDisplayItems(1);

	cmbResolution.clearSelection();

	{
		auto& r = NAS2D::Utility<NAS2D::Renderer>::get();
		auto resolutions = r.getDisplayModes();
		const auto currentResolution = r.getWindowClientArea();
		const auto closestResolution = r.getClosestMatchingDisplayMode(NAS2D::DisplayDesc{currentResolution.x, currentResolution.y});
		const auto s = resolutions.size();
		for (auto i = std::size_t{0}; i < s; ++i)
		{
			const auto& curr_resolution = resolutions[i];
			cmbResolution.addItem(curr_resolution);
			if (curr_resolution == closestResolution)
			{
				currentResolutionSelection = i;
			}
		}
	}
	cmbResolution.currentSelection(currentResolutionSelection);
	cmbResolution.selectionChanged().connect(this, &MainMenuOptions::onVideoOptionsChanged);

	lblFullscreen.size(0, 0);

	cbxFullscreen.size(200, 0);
	cbxFullscreen.click().connect(this, &MainMenuOptions::onVideoOptionsChanged);

	lblSkipSplash.size(0, 0);

	cbxSkipSplash.size(200, 0);
	cbxSkipSplash.click().connect(this, &MainMenuOptions::onOptionsChanged);

	lblStartMaximized.size(0, 0);

	cbxStartMaximized.size(200, 0);
	cbxStartMaximized.click().connect(this, &MainMenuOptions::onOptionsChanged);

	lblVSync.size(0, 0);

	cbxVSync.size(200,0);
	cbxVSync.click().connect(this, &MainMenuOptions::onVideoOptionsChanged);

	lblSoundVolume.size(0, 0);

	sldrSoundVolume.size(250, 10);
	sldrSoundVolume.displayPosition(true);
	sldrSoundVolume.length(100.0f);
	sldrSoundVolume.changeThumbPosition(sldrSoundVolume.length());
	sldrSoundVolume.enabled(true);
	sldrSoundVolume.visible(true);
	sldrSoundVolume.change().connect(this, &MainMenuOptions::onSoundVolumeChanged);

	lblMusicVolume.size(0, 0);

	sldrMusicVolume.size(250, 10);
	sldrMusicVolume.displayPosition(true);
	sldrMusicVolume.length(100.0f);
	sldrMusicVolume.changeThumbPosition(sldrMusicVolume.length());
	sldrMusicVolume.enabled(true);
	sldrMusicVolume.visible(true);
	sldrMusicVolume.change().connect(this, &MainMenuOptions::onMusicVolumeChanged);

	btnApply.size(100, 25);
	btnApply.click().connect(this, &MainMenuOptions::btnApplyClicked);
	btnApply.enabled(false);

	btnCancel.size(100, 25);
	btnCancel.click().connect(this, &MainMenuOptions::btnCancelClicked);

	btnOk.size(100, 25);
	btnOk.click().connect(this, &MainMenuOptions::btnOkClicked);

	pnlControls.add(&lblResolution, left_edge, top_edge);
	pnlControls.add(&cmbResolution, right_edge - cmbResolution.width() - element_spacing, top_edge);
	pnlControls.add(&lblFullscreen, left_edge, lblResolution.rect().startPoint().y() + element_spacing);
	pnlControls.add(&cbxFullscreen, right_edge - cbxFullscreen.width(), cmbResolution.rect().endPoint().y() + element_spacing);
	pnlControls.add(&lblVSync, left_edge, lblFullscreen.rect().startPoint().y() + element_spacing);
	pnlControls.add(&cbxVSync, right_edge - cbxVSync.width(), lblVSync.rect().startPoint().y() + element_spacing);
	pnlControls.add(&lblSoundVolume, left_edge, lblVSync.rect().startPoint().y() + element_spacing);
	pnlControls.add(&sldrSoundVolume, right_edge - sldrSoundVolume.width() - border_right_width, lblSoundVolume.rect().startPoint().y() - sldrSoundVolume.height() * 0.5f);
	pnlControls.add(&lblMusicVolume, left_edge, lblSoundVolume.rect().startPoint().y() + element_spacing);
	pnlControls.add(&sldrMusicVolume, right_edge - sldrMusicVolume.width() - border_right_width, lblMusicVolume.rect().startPoint().y() - sldrMusicVolume.height());
	pnlControls.add(&lblSkipSplash, left_edge, lblMusicVolume.rect().startPoint().y() + element_spacing);
	pnlControls.add(&cbxSkipSplash, right_edge - cbxSkipSplash.width(), sldrMusicVolume.rect().startPoint().y() + element_spacing);
	pnlControls.add(&lblStartMaximized, left_edge, lblSkipSplash.rect().startPoint().y() + element_spacing);
	pnlControls.add(&cbxStartMaximized, right_edge - cbxStartMaximized.width(), cbxSkipSplash.rect().startPoint().y() + element_spacing);
	add(&pnlControls, 0, 0);

	pnlButtons.add(&btnOk, 0, 0);
	pnlButtons.add(&btnCancel, btnOk.rect().endPoint().x() + element_spacing, 0);
	pnlButtons.add(&btnApply, btnCancel.rect().endPoint().x() + element_spacing, 0);
	add(&pnlButtons, left_edge + pnlButtons.width() * 0.25f, pnlControls.height() + element_spacing);//right_edge - pnlButtons.width(), pnlControls.rect().endPoint().y() + element_spacing);

	anchored(true);

	lblResolutionChanged.size(250, 15);
	btnResolutionChangedOk.size(100, 25);
	btnResolutionChangedOk.click().connect(this, &MainMenuOptions::btnResolutionOkClicked);

	dlgResolutionChanged.anchored(true);
	dlgResolutionChanged.size(500, 85);
	dlgResolutionChanged.add(&lblResolutionChanged, dlgResolutionChanged.width() * 0.5f - lblResolutionChanged.width() * 0.5f, 10 + dlgResolutionChanged.height() * 0.25f - lblResolutionChanged.height() * 0.5f);
	dlgResolutionChanged.add(&btnResolutionChangedOk, dlgResolutionChanged.width() * 0.5f - btnResolutionChangedOk.width() * 0.5f, dlgResolutionChanged.height() - 5 - btnResolutionChangedOk.height());
	add(&dlgResolutionChanged, positionX() + (width * 0.5f) - (dlgResolutionChanged.width() * 0.5f), positionY() + (height * 0.5f) - (dlgResolutionChanged.height() * 0.5f));

	setControlsFromConfig();
	inInit = false;
}


void MainMenuOptions::visibilityChanged(bool visible)
{
	if (visible)
	{
		enableControls();
		inInit = true;
		setControlsFromConfig();
	}
	else
	{
		disableControls();
	}
}

void MainMenuOptions::btnOkClicked() {
	setConfigFromControls();
	optionsChanged = false;
	saveOptions();
	applyVideoChanges();
	hide();
}

void MainMenuOptions::btnResolutionOkClicked()
{
	dlgResolutionChanged.hide();
}

void MainMenuOptions::btnCancelClicked()
{
	hide();
}

void MainMenuOptions::btnApplyClicked()
{
	if (btnApply.enabled())
	{
		setConfigFromControls();
		optionsChanged = false;
		saveOptions();
		applyVideoChanges();
	}
}

void MainMenuOptions::saveOptions()
{
	auto& cf = NAS2D::Utility<NAS2D::Configuration>::get();
	cf.save();
}

void MainMenuOptions::loadOptions()
{
	auto& cf = NAS2D::Utility<NAS2D::Configuration>::get();
	cf.load("config.xml");
}

void MainMenuOptions::applyVideoChanges()
{
	if (videoOptionsChanged)
	{
		auto& cf = NAS2D::Utility<NAS2D::Configuration>::get();
		const auto& txt = cmbResolution.selectionText();
		const auto whd = NAS2D::split(txt, 'x');
		const auto width = std::stoi(whd[0]);
		const auto height = std::stoi(whd[1]);
		const auto depth = std::stoi(whd[2]);
		cf.graphicsWidth(width);
		cf.graphicsHeight(height);
		cf.graphicsColorDepth(depth);
		cf.vsync(cbxVSync.checked());
		saveOptions();
		dlgResolutionChanged.show();
		this->bringToFront(&dlgResolutionChanged);
	}
}

void MainMenuOptions::onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier /*mod*/, bool /*repeat*/)
{
	if (key == NAS2D::EventHandler::KeyCode::KEY_ESCAPE)
	{
		btnCancelClicked();
	}
}

void MainMenuOptions::onSoundVolumeChanged(float /*newValue*/)
{
	onOptionsChanged();
}

void MainMenuOptions::onMusicVolumeChanged(float /*newValue*/)
{
	onOptionsChanged();
}

void MainMenuOptions::onOptionsChanged()
{
	if (!inInit)
	{
		optionsChanged = true;
		btnApply.enabled(true);
	}
}

void MainMenuOptions::onVideoOptionsChanged()
{
	if (!inInit)
	{
		optionsChanged = true;
		videoOptionsChanged = true;
	}
}

void MainMenuOptions::enableControls()
{
	pnlControls.enabled(true);
}

void MainMenuOptions::disableControls()
{
	pnlControls.enabled(false);
}

void MainMenuOptions::enableButtons()
{
	pnlButtons.enabled(true);
}

void MainMenuOptions::disableButtons()
{
	pnlButtons.enabled(false);
}

void MainMenuOptions::update()
{
	if (!visible()) { return; }

	//Calls update on the panel controls, which call update on their children.
	Window::update();

	if (optionsChanged)
	{
		optionsChanged = false;
		btnApply.enabled(true);
		setConfigFromControls();
	}
}

void MainMenuOptions::setConfigFromControls()
{
	auto& cf = NAS2D::Utility<NAS2D::Configuration>::get();
	cf.audioSfxVolume(static_cast<int>(std::floor((sldrSoundVolume.thumbPosition() / sldrSoundVolume.length()) * 128.0f)));
	cf.audioMusicVolume(static_cast<int>(std::floor((sldrMusicVolume.thumbPosition() / sldrMusicVolume.length()) * 128.0f)));

	cf.vsync(cbxVSync.checked());
	cf.fullscreen(cbxFullscreen.checked());
	cf.option("skip-splash", (cbxSkipSplash.checked() ? "true" : "false"));
	cf.option("maximized", (cbxStartMaximized.checked() ? "true" : "false"));

	auto& mixer = NAS2D::Utility<NAS2D::Mixer>::get();
	mixer.musicVolume(cf.audioMusicVolume());
	mixer.soundVolume(cf.audioSfxVolume());
}

void MainMenuOptions::setControlsFromConfig()
{
	if (inInit) { return; }
	auto& cf = NAS2D::Utility<NAS2D::Configuration>::get();
	sldrSoundVolume.thumbPosition(std::floor((cf.audioSfxVolume() / 128.0f) * sldrSoundVolume.length()));
	sldrMusicVolume.thumbPosition(std::floor((cf.audioMusicVolume() / 128.0f) * sldrMusicVolume.length()));

	auto& mixer = NAS2D::Utility<NAS2D::Mixer>::get();
	mixer.musicVolume(cf.audioMusicVolume());
	mixer.soundVolume(cf.audioSfxVolume());

	cbxVSync.checked(cf.vsync());
	cbxFullscreen.checked(cf.vsync());

	const auto skipsplash_str = NAS2D::toLowercase(cf.option("skip-splash"));
	const auto maximized_str = NAS2D::toLowercase(cf.option("maximized"));
	cbxSkipSplash.checked(skipsplash_str == "true" ? true : (skipsplash_str == "false" ? false : false));
	cbxStartMaximized.checked(maximized_str == "true" ? true : (maximized_str == "false" ? false : false));
}
