#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>
#include <libControls/TextField.h>
#include <libControls/ListBox.h>
#include <libControls/Label.h>

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
	using FileSaveSignal = NAS2D::Signal<const std::string&>;
	using FileLoadSignal = NAS2D::Signal<const std::string&>;

	FileIo();
	~FileIo() override;

	void showOpen(const std::string& directory);
	void showSave(const std::string& directory);

	FileOperationSignal::Source& fileOperation() { return mSignal; }
	FileSaveSignal::Source& fileSaveSignal() { return mSaveSignal; }
	FileLoadSignal::Source& fileLoadSignal() { return mLoadSignal; }

	void update() override;

protected:
	void onDoubleClick(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier mod, bool repeat);

private:
	void scanDirectory(const std::string& directory);
	void onOpenFolder() const;
	void onClose();
	void onFileIo();
	void onFileDelete();

	void onFileSelect();
	void onFileNameChange(TextControl* control);

	FileOperationSignal mSignal;
	FileSaveSignal mSaveSignal;
	FileLoadSignal mLoadSignal;

	FileOperation mMode{FileOperation::Load};

	std::string mScanPath;

	Button mOpenSaveFolder{"Open Save Folder", {this, &FileIo::onOpenFolder}};
	Button mClose{"Cancel", {this, &FileIo::onClose}};
	Button mFileOperation{"FileOp", {this, &FileIo::onFileIo}};
	Button mDeleteFile{"Delete", {this, &FileIo::onFileDelete}};

	TextField mFileName;

	ListBox<> mListBox;
};
