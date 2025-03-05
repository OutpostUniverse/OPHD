#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>
#include <libControls/TextField.h>
#include <libControls/ListBox.h>
#include <libControls/Label.h>

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/Signal/Delegate.h>
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

	using FileSaveSignal = NAS2D::Signal<const std::string&>;
	using FileLoadDelegate = NAS2D::Delegate<void(const std::string&)>;
	using FileSaveDelegate = NAS2D::Delegate<void(const std::string&)>;

	FileIo(FileLoadDelegate);
	FileIo(FileLoadDelegate, FileSaveDelegate);
	~FileIo() override;

	void showOpen(const std::string& directory);
	void showSave(const std::string& directory);

	FileSaveSignal::Source& fileSaveSignal() { return mSaveSignal; }

	void update() override;

protected:
	void scanDirectory(const std::string& directory);

	void onDoubleClick(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier mod, bool repeat);

	void onOpenFolder() const;
	void onFileSelect();
	void onFileNameChange(TextControl* control);
	void onClose();
	void onFileIo();
	void onFileDelete();

private:
	FileSaveSignal mSaveSignal;
	FileLoadDelegate mFileLoadDelegate;
	FileSaveDelegate mFileSaveDelegate;

	FileOperation mMode;

	std::string mScanPath;

	Button mOpenSaveFolder;
	Button mCancel;
	Button mFileOperation;
	Button mDeleteFile;

	TextField mFileName;

	ListBox<> mListBox;
};
