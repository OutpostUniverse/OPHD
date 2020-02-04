#pragma once

#include "UI.h"

#include "NAS2D/Signal.h"
#include "NAS2D/EventHandler.h"


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

	void setMode(FileOperation fileOp);
	void scanDirectory(const std::string& directory);

	FileOperationCallback& fileOperation() { return mCallback; }

	virtual void update() final;

protected:
	void init();

	void onDoubleClick(NAS2D::EventHandler::MouseButton button, int x, int y);
	void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);

private:
	FileIo(const FileIo&) = delete;
	FileIo& operator=(const FileIo&) = delete;

private:
	void btnCloseClicked();
	void btnFileIoClicked();

	void fileSelected();
	void fileNameModified(Control* control);

private:
	FileOperationCallback	mCallback;

	FileOperation			mMode;

	Button					btnClose;
	Button					btnFileOp;

	TextField				txtFileName;

	ListBox					mListBox;
};
