#pragma once

#include "Core/Window.h"
#include "Core/Button.h"
#include "Core/TextField.h"
#include "Core/ListBox.h"
#include "Core/Label.h"

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>


class FileIo : public Window
{
public:
	enum class FileOperation
	{
		Load,
		Save
	};

	using FileOperationSignal = NAS2D::Signal<const std::string&, FileOperation>;

	FileIo();
	~FileIo() override;

	void setMode(FileOperation fileOp);
	void scanDirectory(const std::string& directory);

	FileOperationSignal::Source& fileOperation() { return mSignal; }

	void update() override;

protected:
	void onDoubleClick(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position);
	void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);

private:
	void onOpenFolder() const;
	void onClose();
	void onFileIo();
	void onFileDelete();

	void onFileSelect();
	void onFileNameChange(TextControl* control);

	FileOperationSignal mSignal;

	FileOperation mMode;

	std::string mScanPath;

	Button mOpenSaveFolder;
	Button btnClose;
	Button btnFileOp;
	Button btnFileDelete;

	TextField txtFileName;

	ListBox<> mListBox;
};
