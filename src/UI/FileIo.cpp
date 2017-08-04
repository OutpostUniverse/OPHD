#include "FileIo.h"

#include "../Constants.h"

/**
 * D'tor
 */
FileIo::FileIo(Font& font) : mBold("fonts/opensans-bold.ttf", 10)
{
	Utility<EventHandler>::get().mouseDoubleClick().connect(this, &FileIo::onDoubleClick);

	Control::font(font);
	text("File I/O");
	init();
}


/**
 * D'tor
 */
FileIo::~FileIo()
{
	Utility<EventHandler>::get().mouseDoubleClick().disconnect(this, &FileIo::onDoubleClick);
}


/**
 * 
 */
void FileIo::init()
{
	position(0, 0);
	size(500, 350);

	addControl("btnFileOp", &btnFileOp, 445, 325);
	btnFileOp.font(font());
	btnFileOp.text("FileOp");
	btnFileOp.size(50, 20);
	btnFileOp.click().connect(this, &FileIo::btnFileIoClicked);
	btnFileOp.enabled(false);

	addControl("btnClose", &btnClose, 390, 325);
	btnClose.font(font());
	btnClose.text("Cancel");
	btnClose.size(50, 20);
	btnClose.click().connect(this, &FileIo::btnCloseClicked);

	addControl("txtFileName", &txtFileName, 5, 302);
	txtFileName.font(font());
	txtFileName.size(490, 18);
	txtFileName.maxCharacters(50);
	txtFileName.textChanged().connect(this,&FileIo::fileNameModified);

	addControl("mListBox", &mListBox, 5, 25);
	mListBox.font(font());
	mListBox.size(490, 273);
	mListBox.visible(true);
	mListBox.selectionChanged().connect(this, &FileIo::fileSelected);
}


void FileIo::onDoubleClick(EventHandler::MouseButton button, int x, int y)
{
	if (isPointInRect(x, y, mListBox.rect().x(), mListBox.rect().y(), mListBox.rect().width(), mListBox.rect().height()))
	{
		if (mListBox.currentHighlight() != constants::NO_SELECTION && !txtFileName.empty())
		{
			btnFileIoClicked();
		}
	}
}


/**
 * 
 */
void FileIo::setMode(FileOperation _m)
{
	mMode = _m;
	mMode == FILE_LOAD ? text("Load Game") : text("Save Game");
	mMode == FILE_LOAD ? btnFileOp.text("Load") : btnFileOp.text("Save");
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
	string sFile = _ctrl->text();

	if (sFile == "")	// no blank filename
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
void FileIo::update()
{
	if (!visible())
		return;

	Window::update();
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
