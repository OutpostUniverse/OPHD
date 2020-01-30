// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "FileIo.h"

#include "../Constants.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Filesystem.h"
#include "NAS2D/MathUtils.h"

using namespace NAS2D;


/**
 * D'tor
 */
FileIo::FileIo()
{
	Utility<EventHandler>::get().mouseDoubleClick().connect(this, &FileIo::onDoubleClick);
	Utility<EventHandler>::get().keyDown().connect(this, &FileIo::onKeyDown);

	text("File I/O");
	init();
}


/**
 * D'tor
 */
FileIo::~FileIo()
{
	Utility<EventHandler>::get().mouseDoubleClick().disconnect(this, &FileIo::onDoubleClick);
	Utility<EventHandler>::get().keyDown().disconnect(this, &FileIo::onKeyDown);
}


/**
 * 
 */
void FileIo::init()
{
	size(500, 350);

	add(&btnFileOp, 445, 325);
	btnFileOp.text("FileOp");
	btnFileOp.size(50, 20);
	btnFileOp.click().connect(this, &FileIo::btnFileIoClicked);
	btnFileOp.enabled(false);

	add(&btnClose, 390, 325);
	btnClose.text("Cancel");
	btnClose.size(50, 20);
	btnClose.click().connect(this, &FileIo::btnCloseClicked);

	add(&txtFileName, 5, 302);
	txtFileName.size(490, 18);
	txtFileName.maxCharacters(50);
	txtFileName.textChanged().connect(this,&FileIo::fileNameModified);

	add(&mListBox, 5, 25);
	mListBox.size(490, 273);
	mListBox.visible(true);
	mListBox.selectionChanged().connect(this, &FileIo::fileSelected);
}


/**
 * Event handler for mouse double click.
 */
void FileIo::onDoubleClick(EventHandler::MouseButton /*button*/, int x, int y)
{
	if (!visible()) { return; }	// ignore key presses when hidden.

	const Rectangle_2d& _rect = mListBox.rect();
	if (isPointInRect(x, y, _rect.x(), _rect.y(), _rect.width(), _rect.height()))
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
	if (!visible()) { return; }	// ignore key presses when hidden.

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


/**
 * 
 */
void FileIo::setMode(FileOperation _m)
{
	mMode = _m; 
	text(mMode == FILE_LOAD ? constants::WINDOW_FILEIO_TITLE_LOAD : constants::WINDOW_FILEIO_TITLE_SAVE);
	btnFileOp.text(mMode == FILE_LOAD ? constants::WINDOW_FILEIO_LOAD : constants::WINDOW_FILEIO_SAVE);
}


/**
 * 
 */
void FileIo::scanDirectory(const std::string& _dir)
{
	Filesystem& f = Utility<Filesystem>::get();
	StringList dirList = f.directoryList(_dir);

	mListBox.dropAllItems();

	for (size_t i = 0; i < dirList.size(); ++i)
	{
		if (!f.isDirectory(_dir + dirList[i]))
		{
			dirList[i].resize(dirList[i].size() - 4);	// Assumes a file save extension of 3 characters.
														// This is a naive approach.
			mListBox.addItem(dirList[i]);
		}
	}
	mListBox.sort();
}


/**
 * 
 */
void FileIo::fileSelected()
{
	txtFileName.text(mListBox.selectionText());
}


/**
 * 
 */
void FileIo::fileNameModified(Control* _ctrl)
{
	std::string sFile = _ctrl->text();

	if (sFile.empty())	// no blank filename
		btnFileOp.enabled(false);
	else if ((int)sFile.find('\\')>-1)	// no \ in the filename
		btnFileOp.enabled(false);
	else if ((int)sFile.find('/')>-1)	// no / in the filename
		btnFileOp.enabled(false);
	else if ((int)sFile.find(':')>-1)	// no : in the filename
		btnFileOp.enabled(false);
	else if ((int)sFile.find('*')>-1)	// no * in the filename
		btnFileOp.enabled(false);
	else if ((int)sFile.find('?')>-1)	// no ? in the filename
		btnFileOp.enabled(false);
	else if ((int)sFile.find('"')>-1)	// no " in the filename
		btnFileOp.enabled(false);
	else if ((int)sFile.find('<')>-1)	// no < in the filename
		btnFileOp.enabled(false);
	else if ((int)sFile.find('>')>-1)	// no > in the filename
		btnFileOp.enabled(false);
	else if ((int)sFile.find('|')>-1)	// no | in the filename
		btnFileOp.enabled(false);
	else
		btnFileOp.enabled(true);
}


/**
 * 
 */
void FileIo::btnCloseClicked()
{
	visible(false);
	txtFileName.text("");
	txtFileName.resetCursorPosition();
}


/**
 * 
 */
void FileIo::btnFileIoClicked()
{
	mCallback(txtFileName.text() , mMode);
	txtFileName.text("");
	txtFileName.resetCursorPosition();
	btnFileOp.enabled(false);
}


/**
 * 
 */
void FileIo::update()
{
	if (!visible()) { return; }

	Window::update();
}
