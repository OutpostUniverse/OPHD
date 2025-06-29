#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>
#include <libControls/TextField.h>
#include <libControls/ListBox.h>

#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/Math/Point.h>


namespace NAS2D
{
	enum class KeyModifier : uint16_t;
	enum class KeyCode : uint32_t;
	enum class MouseButton;
}


class FileIo : public Window
{
public:
	enum class FileOperation
	{
		Load,
		Save
	};

	using FileLoadDelegate = NAS2D::Delegate<void(const std::string&)>;
	using FileSaveDelegate = NAS2D::Delegate<void(const std::string&)>;

	FileIo(FileLoadDelegate);
	FileIo(FileLoadDelegate, FileSaveDelegate);
	~FileIo() override;

	void showOpen(const std::string& directory);
	void showSave(const std::string& directory);

protected:
	void scanDirectory(const std::string& directory);

	void onDoubleClick(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier mod, bool repeat);

	void onOpenFolder() const;
	void onFileSelect();
	void onFileNameChange(TextField& control);
	void onClose();
	void onFileIo();
	void onFileDelete();

private:
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
