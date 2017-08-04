#pragma once

#include "UI.h"


class FileIo : public Window
{
public:

	enum FileOperation
	{
		FILE_LOAD,
		FILE_SAVE
	};

	typedef NAS2D::Signals::Signal2<const std::string&, FileOperation> FileOperationCallback;

	void setMode(FileOperation _m);

	void scanDirectory(const std::string& _dir);

public:

	FileIo(Font& font);
	virtual ~FileIo();

	virtual void update();

	FileOperationCallback& fileOperation() { return mCallback; }

protected:

	virtual void init();

private:

	void btnCloseClicked();
	void btnFileIoClicked();

	void fileSelected();
	void fileNameModified(Control* _ctrl);

	FileIo();
	FileIo(const FileIo&);
	FileIo& operator=(const FileIo&);

	FileOperationCallback	mCallback;

	FileOperation			mMode;

	Button		btnClose;
	Button		btnFileOp;

	TextField	txtFileName;

	ListBox		mListBox;

	Font		mBold;
};
