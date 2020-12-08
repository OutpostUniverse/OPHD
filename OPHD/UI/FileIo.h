#pragma once

#include "Core/Window.h"
#include "Core/Button.h"
#include "Core/TextField.h"
#include "Core/ListBox.h"

#include <NAS2D/Signal.h>
#include <NAS2D/EventHandler.h>


class FileIo : public Window
{
public:
	enum FileOperation
	{
		FILE_LOAD,
		FILE_SAVE
	};

	using FileOperationCallback = NAS2D::Signals::Signal<const std::string&, FileOperation>;

public:
	FileIo();
	~FileIo() override;

	void setMode(FileOperation fileOp);
	void scanDirectory(const std::string& directory);

	FileOperationCallback& fileOperation() { return mCallback; }

	void update() override;

protected:
	void onDoubleClick(NAS2D::EventHandler::MouseButton button, int x, int y);
	void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);

private:
	FileIo(const FileIo&) = delete;
	FileIo& operator=(const FileIo&) = delete;

private:
	void btnCloseClicked();
	void btnFileIoClicked();

	void fileSelected();
	void fileNameModified(TextControl* control);

private:
	FileOperationCallback mCallback;

	FileOperation mMode;

	Button btnClose;
	Button btnFileOp;

	TextField txtFileName;

	ListBox mListBox;
};
