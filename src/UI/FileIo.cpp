#include "FileIo.h"

#include "../Constants.h"

FileIo::FileIo(Font& font) : mBold("fonts/ui-bold.png", 7, 9, 0)
{
	Control::font(font);
	text("File I/O");
	init();
}


FileIo::~FileIo()
{
}


void FileIo::init()
{
	position(0, 0);
	size(500, 350);

	addControl("btnFileOp", &btnFileOp, 445, 325);
	btnFileOp.font(font());
	btnFileOp.text("FileOp");
	btnFileOp.size(50, 20);
	btnFileOp.click().Connect(this, &FileIo::btnFileIoClicked);

	addControl("btnClose", &btnClose, 390, 325);
	btnClose.font(font());
	btnClose.text("Cancel");
	btnClose.size(50, 20);
	btnClose.click().Connect(this, &FileIo::btnCloseClicked);

	addControl("txtFileName", &txtFileName, 5, 302);
	txtFileName.font(font());
	txtFileName.size(490, 18);
	txtFileName.textChanged().Connect(this,&FileIo::fileNameModified);

	addControl("mnuFileList", &mnuFileList, 5, 25);
	mnuFileList.font(font());
	mnuFileList.size(490, 300);
	mnuFileList.visible(true);
	mnuFileList.selectionChanged().Connect(this, &FileIo::fileSelected);
}


void FileIo::setMode(FileOperation _m)
{
	mMode = _m;
	mMode == FILE_LOAD ? text("Load Game") : text("Save Game");
	mMode == FILE_LOAD ? btnFileOp.text("Load") : btnFileOp.text("Save");
}


void FileIo::scanDirectory(const std::string& _dir)
{
	Filesystem& f = Utility<Filesystem>::get();
	StringList dirList = f.directoryList(_dir);

	mnuFileList.dropAllItems();

	for (size_t i = 0; i < dirList.size(); ++i)
	{
		if (!f.isDirectory(_dir + dirList[i]))
		{
			dirList[i].resize(dirList[i].size() - 4);	// Assumes a file save extension of 3 characters.
														// This is a naive approach.
			mnuFileList.addItem(dirList[i]);
		}
	}
}


void FileIo::fileSelected()
{
	txtFileName.text(mnuFileList.selectionText());
}


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


void FileIo::update()
{
	if (!visible())
		return;

	Window::update();

	Renderer& r = Utility<Renderer>::get();
}


void FileIo::btnCloseClicked()
{
	visible(false);
	txtFileName.text("");
	txtFileName.resetCursorPosition();
}


void FileIo::btnFileIoClicked()
{
	mCallback(txtFileName.text() , mMode);
	txtFileName.text("");
	txtFileName.resetCursorPosition();
}