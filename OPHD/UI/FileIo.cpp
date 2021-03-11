#include "FileIo.h"

#include "../Constants.h"
#include "../Common.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/MathUtils.h>

#include <string>
#include <vector>
#include <algorithm>


using namespace NAS2D;


FileIo::FileIo() :
	Window{"File I/O"},
	btnClose{"Cancel"},
	btnFileOp{"FileOp"},
	btnFileDelete{"Delete"}
{
	Utility<EventHandler>::get().mouseDoubleClick().connect(this, &FileIo::onDoubleClick);
	Utility<EventHandler>::get().keyDown().connect(this, &FileIo::onKeyDown);

	size({500, 350});

	add(btnFileOp, {445, 325});
	btnFileOp.size({50, 20});
	btnFileOp.click().connect(this, &FileIo::btnFileIoClicked);
	btnFileOp.enabled(false);

	add(btnFileDelete, {5, 325});
	btnFileDelete.size({50, 20});
	btnFileDelete.click().connect(this, &FileIo::btnFileDeleteClicked);
	btnFileDelete.enabled(false);

	add(btnClose, {390, 325});
	btnClose.size({50, 20});
	btnClose.click().connect(this, &FileIo::btnCloseClicked);

	add(txtFileName, {5, 302});
	txtFileName.size({490, 18});
	txtFileName.maxCharacters(50);
	txtFileName.textChanged().connect(this, &FileIo::fileNameModified);

	add(mListBox, {5, 25});
	mListBox.size({490, 273});
	mListBox.visible(true);
	mListBox.selectionChanged().connect(this, &FileIo::fileSelected);
}


FileIo::~FileIo()
{
	Utility<EventHandler>::get().mouseDoubleClick().disconnect(this, &FileIo::onDoubleClick);
	Utility<EventHandler>::get().keyDown().disconnect(this, &FileIo::onKeyDown);
}


/**
 * Event handler for mouse double click.
 */
void FileIo::onDoubleClick(EventHandler::MouseButton /*button*/, int x, int y)
{
	if (!visible()) { return; } // ignore key presses when hidden.

	if (mListBox.rect().contains(NAS2D::Point{x, y}))
	{
		if (mListBox.currentHighlight() != constants::NO_SELECTION && !txtFileName.empty())
		{
			btnFileIoClicked();
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
			btnFileIoClicked();
		}
	}
	else if (key == EventHandler::KeyCode::KEY_ESCAPE)
	{
		btnCloseClicked();
	}
}


void FileIo::setMode(FileOperation fileOp)
{
	mMode = fileOp; 
	title(mMode == FileOperation::FILE_LOAD ? constants::WINDOW_FILEIO_TITLE_LOAD : constants::WINDOW_FILEIO_TITLE_SAVE);
	btnFileOp.text(mMode == FileOperation::FILE_LOAD ? constants::WINDOW_FILEIO_LOAD : constants::WINDOW_FILEIO_SAVE);
}


void FileIo::scanDirectory(const std::string& directory)
{
	Filesystem& f = Utility<Filesystem>::get();
	std::vector<std::string> dirList = f.directoryList(directory);
	std::sort(dirList.begin(), dirList.end());

	mListBox.clear();
	for (auto& dir : dirList)
	{
		if (!f.isDirectory(directory + dir))
		{
			// FixMe: Naive approach: Assumes a file save extension of 3 characters.
			dir.resize(dir.size() - 4);
			mListBox.add(dir);
		}
	}
}


void FileIo::fileSelected()
{
	txtFileName.text(mListBox.isItemSelected() ? mListBox.selected().text : "");
}


void FileIo::fileNameModified(TextControl* control)
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


void FileIo::btnCloseClicked()
{
	visible(false);
	txtFileName.text("");
	txtFileName.resetCursorPosition();
}


void FileIo::btnFileIoClicked()
{
	mCallback(txtFileName.text(), mMode);
	txtFileName.text("");
	txtFileName.resetCursorPosition();
	btnFileOp.enabled(false);
}

void FileIo::btnFileDeleteClicked()
{
	std::string filename = constants::SAVE_GAME_PATH + txtFileName.text() + ".xml";

	try
	{
		if(doYesNoMessage(constants::WINDOW_FILEIO_TITLE_DELETE, "Are you sure you want to delete " + txtFileName.text() + "?"))
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
	scanDirectory(constants::SAVE_GAME_PATH);
}



void FileIo::update()
{
	if (!visible()) { return; }

	Window::update();
}
