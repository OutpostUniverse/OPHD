#include "FileIo.h"

#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../Common.h"
#include "../ShellOpenPath.h"

#include "MessageBox.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/Math/MathUtils.h>

#include <string>
#include <vector>
#include <algorithm>


using namespace NAS2D;


FileIo::FileIo() : Window{"File I/O"}
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseDoubleClick().connect({this, &FileIo::onDoubleClick});
	eventHandler.keyDown().connect({this, &FileIo::onKeyDown});

	size({700, 350});

	add(mOpenSaveFolder, {590, 22});
	mOpenSaveFolder.size({105, 20});

	add(mFileOperation, {645, 325});
	mFileOperation.size({50, 20});
	mFileOperation.enabled(false);

	add(mDeleteFile, {5, 325});
	mDeleteFile.size({50, 20});
	mDeleteFile.enabled(false);

	add(mClose, {590, 325});
	mClose.size({50, 20});

	add(mFileName, {5, 302});
	mFileName.size({690, 18});
	mFileName.maxCharacters(50);
	mFileName.textChanged().connect({this, &FileIo::onFileNameChange});

	add(mListBox, {5, 45});
	mListBox.size({690, 253});
	mListBox.visible(true);
	mListBox.selectionChanged().connect({this, &FileIo::onFileSelect});
}


FileIo::~FileIo()
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseDoubleClick().disconnect({this, &FileIo::onDoubleClick});
	eventHandler.keyDown().disconnect({this, &FileIo::onKeyDown});
}


void FileIo::onDoubleClick(EventHandler::MouseButton /*button*/, NAS2D::Point<int> position)
{
	if (!visible()) { return; } // ignore key presses when hidden.

	if (mListBox.rect().contains(position))
	{
		if (mListBox.isItemSelected() && !mFileName.empty())
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
		if (!mFileName.empty())
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
	mFileOperation.text(mMode == FileOperation::Load ? constants::WindowFileIoLoad : constants::WindowFileIoSave);
}


void FileIo::scanDirectory(const std::string& directory)
{
	mScanPath = (Utility<Filesystem>::get().prefPath() / directory).string();

	const auto& filesystem = Utility<Filesystem>::get();
	auto dirList = filesystem.directoryList(directory);
	std::sort(dirList.begin(), dirList.end());

	mListBox.clear();
	for (auto& dir : dirList)
	{
		if (!filesystem.isDirectory(directory + dir.string()))
		{
			mListBox.add(dir.stem().string());
		}
	}
}


void FileIo::onFileSelect()
{
	mFileName.text(mListBox.isItemSelected() ? mListBox.selected().text : "");
}


void FileIo::onFileNameChange(TextControl* control)
{
	std::string sFile = control->text();

	const std::string RestrictedFilenameChars = "\\/:*?\"<>|";

	if (sFile.empty()) // no blank filename
	{
		mFileOperation.enabled(false);
		mDeleteFile.enabled(false);
	}
	else if (sFile.find_first_of(RestrictedFilenameChars) != std::string::npos)
	{
		mFileOperation.enabled(false);
		mDeleteFile.enabled(false);
	}
	else
	{
		mFileOperation.enabled(true);
		mDeleteFile.enabled(true);
	}
}


void FileIo::onOpenFolder() const
{
	shellOpenPath(mScanPath);
}


void FileIo::onClose()
{
	visible(false);
	mFileName.text("");
	mFileName.resetCursorPosition();
}


void FileIo::onFileIo()
{
	mSignal(mFileName.text(), mMode);
	mFileName.text("");
	mFileName.resetCursorPosition();
	mFileOperation.enabled(false);
}

void FileIo::onFileDelete()
{
	std::string filename = constants::SaveGamePath + mFileName.text() + ".xml";

	try
	{
		if(doYesNoMessage(constants::WindowFileIoTitleDelete, "Are you sure you want to delete " + mFileName.text() + "?"))
		{
			Utility<Filesystem>::get().del(filename);
		}
	}
	catch(const std::exception& e)
	{
		doNonFatalErrorMessage("Delete Failed", e.what());
	}

	mFileName.text("");
	mFileName.resetCursorPosition();
	mDeleteFile.enabled(false);
	scanDirectory(constants::SaveGamePath);
}



void FileIo::update()
{
	if (!visible()) { return; }

	Window::update();
}
