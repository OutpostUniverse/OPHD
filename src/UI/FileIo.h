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

public:
	FileIo();
	virtual ~FileIo();

	void setMode(FileOperation _m);
	void scanDirectory(const std::string& _dir);

	FileOperationCallback& fileOperation() { return mCallback; }

	virtual void update() final;

protected:
	void init();

	virtual void onDoubleClick(NAS2D::EventHandler::MouseButton button, int x, int y) final;
	virtual void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat) final;

private:
	FileIo(const FileIo&) = delete;
	FileIo& operator=(const FileIo&) = delete;

private:
	void btnCloseClicked();
	void btnFileIoClicked();

	void fileSelected();
	void fileNameModified(Control* _ctrl);

private:
	FileOperationCallback	mCallback;

	FileOperation			mMode;

	Button					btnClose;
	Button					btnFileOp;

	TextField				txtFileName;

	ListBox					mListBox;
};
