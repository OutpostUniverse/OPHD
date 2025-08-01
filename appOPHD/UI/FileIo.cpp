#include "FileIo.h"

#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../ShellOpenPath.h"

#include "MessageBox.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/EnumKeyCode.h>
#include <NAS2D/Math/Point.h>

#include <string>
#include <algorithm>


FileIo::FileIo(FileLoadDelegate fileLoadHandler, FileSaveDelegate fileSaveHandler) :
	Window{"File I/O"},
	mFileLoadHandler{fileLoadHandler},
	mFileSaveHandler{fileSaveHandler},
	mMode{FileOperation::Load},
	mOpenSaveFolder{"Open Save Folder", {this, &FileIo::onOpenFolder}},
	mCancel{"Cancel", {this, &FileIo::onClose}},
	mFileOperation{"FileOp", {this, &FileIo::onFileIo}},
	mDeleteFile{"Delete", {this, &FileIo::onFileDelete}},
	mFileName{50, {this, &FileIo::onFileNameChange}},
	mListBox{{this, &FileIo::onFileSelect}}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseDoubleClick().connect({this, &FileIo::onDoubleClick});
	eventHandler.keyDown().connect({this, &FileIo::onKeyDown});

	mOpenSaveFolder.size({std::max(105, mOpenSaveFolder.size().x + constants::Margin), 20});
	add(mOpenSaveFolder, {5 + 690 - mOpenSaveFolder.size().x, sWindowTitleBarHeight + 2});

	mListBox.size({690, 253});
	mListBox.visible(true);
	add(mListBox, {5, 45});

	mFileName.size({mListBox.size().x, std::max(18, mFileName.size().y)});
	add(mFileName, mListBox.area().crossYPoint() - NAS2D::Point{0, 0} + NAS2D::Vector{0, 4});

	const auto bottomButtonArea = NAS2D::Rectangle{mFileName.area().crossYPoint() + NAS2D::Vector{0, 5}, {mFileName.size().x, 20}};

	mFileOperation.size({50, 20});
	mFileOperation.enabled(false);
	add(mFileOperation, {bottomButtonArea.endPoint().x - mFileOperation.size().x, bottomButtonArea.position.y});

	mDeleteFile.size({std::max(50, mDeleteFile.size().x + constants::Margin), 20});
	mDeleteFile.enabled(false);
	add(mDeleteFile, {bottomButtonArea.position.x, bottomButtonArea.position.y});

	mCancel.size({std::max(50, mCancel.size().x + constants::Margin), 20});
	add(mCancel, {mFileOperation.position().x - mCancel.size().x - 5, bottomButtonArea.position.y});

	size(bottomButtonArea.endPoint() - NAS2D::Point{0, 0} + NAS2D::Vector{5, 5});
}


FileIo::~FileIo()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseDoubleClick().disconnect({this, &FileIo::onDoubleClick});
	eventHandler.keyDown().disconnect({this, &FileIo::onKeyDown});
}


void FileIo::showOpen(const std::string& directory)
{
	scanDirectory(directory);
	mMode = FileOperation::Load;
	title(constants::WindowFileIoTitleLoad);
	mFileOperation.text(constants::WindowFileIoLoad);
	show();
}


void FileIo::showSave(const std::string& directory)
{
	scanDirectory(directory);
	mMode = FileOperation::Save;
	title(constants::WindowFileIoTitleSave);
	mFileOperation.text(constants::WindowFileIoSave);
	show();
}


void FileIo::scanDirectory(const std::string& directory)
{
	mScanPath = (NAS2D::Utility<NAS2D::Filesystem>::get().prefPath() / directory).string();

	const auto& filesystem = NAS2D::Utility<NAS2D::Filesystem>::get();
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


void FileIo::onDoubleClick(NAS2D::MouseButton /*button*/, NAS2D::Point<int> position)
{
	if (!visible()) { return; } // ignore key presses when hidden.

	if (mListBox.area().contains(position))
	{
		if (mListBox.isItemSelected() && !mFileName.isEmpty())
		{
			onFileIo();
		}
	}
}


/**
 * Event handler for Key Down.
 */
void FileIo::onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier /*mod*/, bool /*repeat*/)
{
	if (!visible()) { return; } // ignore key presses when hidden.

	if (key == NAS2D::KeyCode::Enter || key == NAS2D::KeyCode::KeypadEnter)
	{
		if (!mFileName.isEmpty())
		{
			onFileIo();
		}
	}
	else if (key == NAS2D::KeyCode::Escape)
	{
		onClose();
	}
}


void FileIo::onOpenFolder() const
{
	shellOpenPath(mScanPath);
}


void FileIo::onFileSelect()
{
	mFileName.text(mListBox.isItemSelected() ? mListBox.selected().text : "");
}


void FileIo::onFileNameChange(TextField& control)
{
	std::string sFile = control.text();

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


void FileIo::onClose()
{
	visible(false);
	mFileName.clear();
}


void FileIo::onFileIo()
{
	if(mMode == FileOperation::Save)
	{
		if(mFileSaveHandler) { mFileSaveHandler(mFileName.text()); }
	}

	if(mMode == FileOperation::Load)
	{
		mFileLoadHandler(mFileName.text());
	}
	mFileName.clear();
	mFileOperation.enabled(false);
}


void FileIo::onFileDelete()
{
	std::string filename = constants::SaveGamePath + mFileName.text() + ".xml";

	try
	{
		if(doYesNoMessage(constants::WindowFileIoTitleDelete, "Are you sure you want to delete " + mFileName.text() + "?"))
		{
			NAS2D::Utility<NAS2D::Filesystem>::get().del(filename);
		}
	}
	catch(const std::exception& e)
	{
		doNonFatalErrorMessage("Delete Failed", e.what());
	}

	mFileName.clear();
	mDeleteFile.enabled(false);
	scanDirectory(constants::SaveGamePath);
}
