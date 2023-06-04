#pragma once

#include "Control.h"
#include "Label.h"
#include <NAS2D/Signal/Signal.h>
#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/MathUtils.h>
#include <NAS2D/Math/Point.h>

#include <algorithm>
#include <string>
#include <limits>


class RadioButtonGroup : public Control
{
private:
	class RadioButton : public Control
	{
	public:
		RadioButton(RadioButtonGroup& parentContainer, std::string newText, NAS2D::Delegate<void()> delegate);
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
		void onMouseDown(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position);

	private:
		const NAS2D::Font& mFont;
		const NAS2D::Image& mSkin;
		Label mLabel;
		RadioButtonGroup& mParentContainer;
		bool mChecked{false};
		NAS2D::Signal<> mSignal;
	};

public:
	struct ButtonInfo
	{
		std::string name;
		NAS2D::Delegate<void()> delegate;
	};

	static inline constexpr auto NoSelection{std::numeric_limits<std::size_t>::max()};


	RadioButtonGroup() = default;
	RadioButtonGroup(std::vector<ButtonInfo> buttonInfos);

	void clear();

	void select(std::size_t index);
	void select(RadioButtonGroup::RadioButton& button);

	void update() override;
	void draw() const override;

protected:
	void onMove(NAS2D::Vector<int> displacement) override;

private:
	std::size_t mIndex = NoSelection;
	std::vector<RadioButton> mRadioButtons;
};
