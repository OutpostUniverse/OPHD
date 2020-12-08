#include "FileIo.h"

#include "../Constants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/MathUtils.h>

#include <string>
#include <vector>


using namespace NAS2D;


FileIo::FileIo() :
	btnClose{"Cancel"},
	btnFileOp{"FileOp"}
{
	Utility<EventHandler>::get().mouseDoubleClick().connect(this, &FileIo::onDoubleClick);
	Utility<EventHandler>::get().keyDown().connect(this, &FileIo::onKeyDown);

	text("File I/O");
	init();
}


FileIo::~FileIo()
{
	Utility<EventHandler>::get().mouseDoubleClick().disconnect(this, &FileIo::onDoubleClick);
	Utility<EventHandler>::get().keyDown().disconnect(this, &FileIo::onKeyDown);
}


void FileIo::init()
{
	size({500, 350});

	add(btnFileOp, {445, 325});
	btnFileOp.size({50, 20});
	btnFileOp.click().connect(this, &FileIo::btnFileIoClicked);
	btnFileOp.enabled(false);

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
	text(mMode == FileOperation::FILE_LOAD ? constants::WINDOW_FILEIO_TITLE_LOAD : constants::WINDOW_FILEIO_TITLE_SAVE);
	btnFileOp.text(mMode == FileOperation::FILE_LOAD ? constants::WINDOW_FILEIO_LOAD : constants::WINDOW_FILEIO_SAVE);
}


void FileIo::scanDirectory(const std::string& directory)
{
	Filesystem& f = Utility<Filesystem>::get();
	std::vector<std::string> dirList = f.directoryList(directory);

	mListBox.dropAllItems();

	for (auto& dir : dirList)
	{
		if (!f.isDirectory(directory + dir))
		{
			// FixMe: Naive approach: Assumes a file save extension of 3 characters.
			dir.resize(dir.size() - 4);
			mListBox.addItem(dir);
		}
	}
	mListBox.sort();
}


void FileIo::fileSelected()
{
	txtFileName.text(mListBox.selectionText());
}


void FileIo::fileNameModified(TextControl* control)
{
	std::string sFile = control->text();

	const std::string RestrictedFilenameChars = "\\/:*?\"<>|";

	if (sFile.empty()) // no blank filename
		btnFileOp.enabled(false);
	else if (sFile.find_first_of(RestrictedFilenameChars) != std::string::npos)
		btnFileOp.enabled(false);
	else
		btnFileOp.enabled(true);
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


void FileIo::update()
{
	if (!visible()) { return; }

	Window::update();
}
