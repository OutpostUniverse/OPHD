#include "FileIo.h"

#include "../Constants.h"
#include "../Common.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/Math/MathUtils.h>

#include <string>
#include <vector>
#include <algorithm>


using namespace NAS2D;


FileIo::FileIo() :
	Window{"File I/O"},
	btnClose{"Cancel", {this, &FileIo::onClose}},
	btnFileOp{"FileOp", {this, &FileIo::onFileIo}},
	btnFileDelete{"Delete", {this, &FileIo::onFileDelete}}
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseDoubleClick().connect(this, &FileIo::onDoubleClick);
	eventHandler.keyDown().connect(this, &FileIo::onKeyDown);

	size({500, 350});

	add(btnFileOp, {445, 325});
	btnFileOp.size({50, 20});
	btnFileOp.enabled(false);

	add(btnFileDelete, {5, 325});
	btnFileDelete.size({50, 20});
	btnFileDelete.enabled(false);

	add(btnClose, {390, 325});
	btnClose.size({50, 20});

	add(txtFileName, {5, 302});
	txtFileName.size({490, 18});
	txtFileName.maxCharacters(50);
	txtFileName.textChanged().connect(this, &FileIo::onFileNameChange);

	add(mListBox, {5, 25});
	mListBox.size({490, 273});
	mListBox.visible(true);
	mListBox.selectionChanged().connect(this, &FileIo::onFileSelect);
}


FileIo::~FileIo()
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseDoubleClick().disconnect(this, &FileIo::onDoubleClick);
	eventHandler.keyDown().disconnect(this, &FileIo::onKeyDown);
}


/**
 * Event handler for mouse double click.
 */
void FileIo::onDoubleClick(EventHandler::MouseButton /*button*/, int x, int y)
{
	if (!visible()) { return; } // ignore key presses when hidden.

	if (mListBox.rect().contains(NAS2D::Point{x, y}))
	{
		if (mListBox.currentHighlight() != constants::NoSelection && !txtFileName.empty())
		{
			onFileIo();
		}
	}
}


/**
 * Event handler for Key Down.
 */
void FileIo::onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier /*mod*/, bool /*repeat*/)
{
	if (!visible()) { return; } // ignore key presses when hidden.

	if (key == EventHandler::KeyCode::KEY_ENTER || key == EventHandler::KeyCode::KEY_KP_ENTER)
	{
		if (!txtFileName.empty())
		{
			onFileIo();
		}
	}
	else if (key == EventHandler::KeyCode::KEY_ESCAPE)
	{
		onClose();
	}
}


void FileIo::setMode(FileOperation fileOp)
{
	mMode = fileOp;
	title(mMode == FileOperation::Load ? constants::WindowFileIoTitleLoad : constants::WindowFileIoTitleSave);
	btnFileOp.text(mMode == FileOperation::Load ? constants::WindowFileIoLoad : constants::WindowFileIoSave);
}


void FileIo::scanDirectory(const std::string& directory)
{
	const auto& filesystem = Utility<Filesystem>::get();
	std::vector<std::string> dirList = filesystem.directoryList(directory);
	std::sort(dirList.begin(), dirList.end());

	mListBox.clear();
	for (auto& dir : dirList)
	{
		if (!filesystem.isDirectory(directory + dir))
		{
			// FixMe: Naive approach: Assumes a file save extension of 3 characters.
			dir.resize(dir.size() - 4);
			mListBox.add(dir);
		}
	}
}


void FileIo::onFileSelect()
{
	txtFileName.text(mListBox.isItemSelected() ? mListBox.selected().text : "");
}


void FileIo::onFileNameChange(TextControl* control)
{
	std::string sFile = control->text();

	const std::string RestrictedFilenameChars = "\\/:*?\"<>|";

	if (sFile.empty()) // no blank filename
	{
		btnFileOp.enabled(false);
		btnFileDelete.enabled(false);
	}
	else if (sFile.find_first_of(RestrictedFilenameChars) != std::string::npos)
	{
		btnFileOp.enabled(false);
		btnFileDelete.enabled(false);
	}
	else
	{
		btnFileOp.enabled(true);
		btnFileDelete.enabled(true);
	}
}


void FileIo::onClose()
{
	visible(false);
	txtFileName.text("");
	txtFileName.resetCursorPosition();
}


void FileIo::onFileIo()
{
	mSignal(txtFileName.text(), mMode);
	txtFileName.text("");
	txtFileName.resetCursorPosition();
	btnFileOp.enabled(false);
}

void FileIo::onFileDelete()
{
	std::string filename = constants::SaveGamePath + txtFileName.text() + ".xml";

	try
	{
		if(doYesNoMessage(constants::WindowFileIoTitleDelete, "Are you sure you want to delete " + txtFileName.text() + "?"))
		{
			Utility<Filesystem>::get().del(filename);
		}
	}
	catch(const std::exception& e)
	{
		doNonFatalErrorMessage("Delete Failed", e.what());
	}

	txtFileName.text("");
	txtFileName.resetCursorPosition();
	btnFileDelete.enabled(false);
	scanDirectory(constants::SaveGamePath);
}



void FileIo::update()
{
	if (!visible()) { return; }

	Window::update();
}
