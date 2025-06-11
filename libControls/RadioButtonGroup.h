#pragma once

#include "Control.h"
#include "Label.h"

#include <NAS2D/Signal/Delegate.h>

#include <string>
#include <vector>


namespace NAS2D
{
	enum class MouseButton;
	class Image;
}


class RadioButtonGroup : public Control
{
private:
	class RadioButton : public Control
	{
	public:
		RadioButton(RadioButtonGroup& parentContainer, std::string newText);
		~RadioButton() override;

		// TODO: Best to delete these, but they need to exist for now
		// The default methods do not properly handle global event connect/disconnect
		RadioButton(RadioButton&&) = default;

		void checked(bool toggle);
		bool checked() const;

		void text(const std::string& text);
		const std::string& text() const;

		void draw() const override;

	protected:
		void onResize() override;
		void onTextChange();
		void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);

	private:
		const NAS2D::Font& mFont;
		const NAS2D::Image& mSkin;
		Label mLabel;
		RadioButtonGroup& mParentContainer;
		bool mChecked{false};
	};

public:
	static const std::size_t NoSelection;

	using SelectDelegate = NAS2D::Delegate<void(std::size_t)>;


	RadioButtonGroup(std::vector<std::string> options = {}, SelectDelegate selectHandler = {});

	void clear();
	void select(std::size_t index);
	void select(RadioButtonGroup::RadioButton& button);

	void update() override;
	void draw() const override;

protected:
	void onMove(NAS2D::Vector<int> displacement) override;
	void onClearSelection();
	void onSetSelection(std::size_t index);

private:
	const SelectDelegate mSelectHandler;
	std::size_t mIndex = NoSelection;
	std::vector<RadioButton> mRadioButtons;
};
