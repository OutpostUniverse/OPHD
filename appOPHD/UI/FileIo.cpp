#include "FileIo.h"

#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../ShellOpenPath.h"

#include "MessageBox.h"

#include <NAS2D/EnumKeyCode.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>

#include <string>
#include <vector>
#include <algorithm>


using namespace NAS2D;


FileIo::FileIo() :
	Window{"File I/O"},
	mMode{FileOperation::Load},
	mOpenSaveFolder{"Open Save Folder", {this, &FileIo::onOpenFolder}},
	mCancel{"Cancel", {this, &FileIo::onClose}},
	mFileOperation{"FileOp", {this, &FileIo::onFileIo}},
	mDeleteFile{"Delete", {this, &FileIo::onFileDelete}}
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseDoubleClick().connect({this, &FileIo::onDoubleClick});
	eventHandler.keyDown().connect({this, &FileIo::onKeyDown});

	mOpenSaveFolder.size({std::max(105, mOpenSaveFolder.size().x + constants::Margin), 20});
	add(mOpenSaveFolder, {0,0});

	mListBox.size({690, 253});
	mListBox.visible(true);
	mListBox.selectionChanged().connect({this, &FileIo::onFileSelect});
	add(mListBox, {0, 0});

	mFileName.size({mListBox.size().x, std::max(18, mFileName.size().y)});
	mFileName.maxCharacters(50);
	mFileName.textChanged().connect({this, &FileIo::onFileNameChange});
	add(mFileName, {0, 0});

	mFileOperation.size({50, 20});
	mFileOperation.enabled(false);
	add(mFileOperation, {0, 0});

	mDeleteFile.size({std::max(50, mDeleteFile.size().x + constants::Margin), 20});
	mDeleteFile.enabled(false);
	add(mDeleteFile, {0, 0});

	mCancel.size({std::max(50, mCancel.size().x + constants::Margin), 20});
	add(mCancel, {0, 0});

	ControlGroup bottomButtonGroup = ControlGroup{ControlGroup::ControlList{&mFileOperation, &mDeleteFile, &mCancel}};
	ControlGroup verticalGroup = ControlGroup{ControlGroup::ControlList{&mOpenSaveFolder, &mListBox, &mFileName, &bottomButtonGroup}};
	ControlGroup commonJustify = ControlGroup{ControlGroup::ControlList{&mOpenSaveFolder, &mCancel}};
	verticalGroup.applyVerticalLayout();
	verticalGroup.applyOffset({4 , sWindowTitleBarHeight + 2});
	bottomButtonGroup.applyHorizontalLayout();

	size(verticalGroup.calculateBoundary().size + NAS2D::Vector<int>{4, 4});
	commonJustify.applyCommonXJustify(size().x - mOpenSaveFolder.size().x - 4);
}


FileIo::~FileIo()
{
	auto& eventHandler = Utility<EventHandler>::get();
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


void FileIo::onDoubleClick(MouseButton /*button*/, NAS2D::Point<int> position)
{
	if (!visible()) { return; } // ignore key presses when hidden.

	if (mListBox.area().contains(position))
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
void FileIo::onKeyDown(KeyCode key, KeyModifier /*mod*/, bool /*repeat*/)
{
	if (!visible()) { return; } // ignore key presses when hidden.

	if (key == KeyCode::Enter || key == KeyCode::KeypadEnter)
	{
		if (!mFileName.empty())
		{
			onFileIo();
		}
	}
	else if (key == KeyCode::Escape)
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


void FileIo::onClose()
{
	visible(false);
	mFileName.text("");
	mFileName.resetCursorPosition();
}


void FileIo::onFileIo()
{
	if(mMode == FileOperation::Save)
	{
		mSaveSignal(mFileName.text());
	}

	if(mMode == FileOperation::Load)
	{
		mLoadSignal(mFileName.text());
	}
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
