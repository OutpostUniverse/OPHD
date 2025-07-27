// ==================================================================================
// = NAS2D+GUI
// = Copyright © 2008 - 2020, Leeor Dicker
// ==================================================================================
// = This file is part of the NAS2D+GUI library.
// ==================================================================================

#include "TextField.h"

#include "LoadRectangleSkin.h"

#include <NAS2D/EnumKeyCode.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>
#include <NAS2D/Math/Point.h>

#include <locale>
#include <string_view>


namespace
{
	constexpr int fieldPadding = 4;
	constexpr int cursorBlinkDelay = 250;
}


TextField::TextField(std::size_t maxCharacters, TextChangedDelegate textChangedHandler) :
	mFont{getDefaultFont()},
	mSkinNormal{loadRectangleSkin("ui/skin/textbox_normal")},
	mSkinFocus{loadRectangleSkin("ui/skin/textbox_highlight")},
	mMaxCharacters{maxCharacters},
	mTextChangedHandler{textChangedHandler}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().connect({this, &TextField::onMouseDown});
	eventHandler.keyDown().connect({this, &TextField::onKeyDown});
	eventHandler.textInput().connect({this, &TextField::onTextInput});

	size({mFont.width("W") * static_cast<int>(maxCharacters) + fieldPadding * 2, mFont.height() + fieldPadding * 2});
}


TextField::~TextField()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect({this, &TextField::onMouseDown});
	eventHandler.keyDown().disconnect({this, &TextField::onKeyDown});
	eventHandler.textInput().disconnect({this, &TextField::onTextInput});
}


void TextField::text(const std::string& text)
{
	if (mText != text)
	{
		mText = text;
		onTextChange();
	}
}


const std::string& TextField::text() const
{
	return mText;
}


bool TextField::isEmpty() const
{
	return mText.empty();
}


void TextField::clear()
{
	if (!mText.empty())
	{
		mText.clear();
		mCursorCharacterIndex = 0;
		updateScrollPosition();
		onTextChange();
	}
}


void TextField::border(BorderVisibility visibility)
{
	mBorderVisibility = visibility;
}


void TextField::editable(bool editable)
{
	mEditable = editable;
}


bool TextField::editable() const
{
	return mEditable;
}


void TextField::numbersOnly(bool isNumbersOnly)
{
	mNumbersOnly = isNumbersOnly;
}


/**
 * Sets the maximum number of characters allowed in the text field.
 * 
 * \param	count	Number of characters allowed in the field.
 * 
 * \note	Calling this with \c 0 will clear character limit.
 */
void TextField::maxCharacters(std::size_t count)
{
	mMaxCharacters = count;
}


void TextField::update()
{
	if (!visible()) { return; }

	if (mCursorBlinkTimer.elapsedTicks() > cursorBlinkDelay)
	{
		mCursorBlinkTimer.reset();
		mShowCursor = !mShowCursor;
	}

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	draw(renderer);
}


void TextField::updateScrollPosition()
{
	const auto cursorVirtualPixelX = mFont.width(std::string_view{mText}.substr(0, mCursorCharacterIndex));
	const auto viewWidth = mRect.size.x - fieldPadding * 2;

	// Check if cursor is after visible area
	if (mScrollOffsetPixelX <= cursorVirtualPixelX - viewWidth)
	{
		mScrollOffsetPixelX = cursorVirtualPixelX - viewWidth;
	}

	// Check if cursor is before visible area
	if (mScrollOffsetPixelX >= cursorVirtualPixelX)
	{
		mScrollOffsetPixelX = cursorVirtualPixelX - viewWidth / 2;
	}

	if (mScrollOffsetPixelX < 0)
	{
		mScrollOffsetPixelX = 0;
	}

	mCursorOffsetPixelX = cursorVirtualPixelX - mScrollOffsetPixelX;
}


void TextField::draw(NAS2D::Renderer& renderer) const
{
	const auto showFocused = (mBorderVisibility == BorderVisibility::Always) ||
		((mBorderVisibility == BorderVisibility::FocusOnly) && hasFocus() && editable());
	const auto& skin = showFocused ? mSkinFocus : mSkinNormal;
	skin.draw(renderer, mRect);

	if (highlight()) { renderer.drawBox(mRect, NAS2D::Color::Yellow); }

	drawCursor(renderer);

	const auto textDrawArea = mRect.inset(fieldPadding);
	renderer.clipRect(textDrawArea);
	renderer.drawText(mFont, mText, textDrawArea.position + NAS2D::Vector{-mScrollOffsetPixelX, 0}, NAS2D::Color::White);
	renderer.clipRectClear();
}


void TextField::drawCursor(NAS2D::Renderer& renderer) const
{
	if (hasFocus() && editable())
	{
		if (mShowCursor)
		{
			const auto startPosition = mRect.position + NAS2D::Vector{mCursorOffsetPixelX + fieldPadding, fieldPadding};
			const auto endPosition = mRect.position + NAS2D::Vector{mCursorOffsetPixelX + fieldPadding, mRect.size.y - fieldPadding - 1};
			renderer.drawLine(startPosition + NAS2D::Vector{1, 1}, endPosition + NAS2D::Vector{1, 1}, NAS2D::Color::Black);
			renderer.drawLine(startPosition, endPosition, NAS2D::Color::White);
		}
	}
}


void TextField::onMouseDown(NAS2D::MouseButton /*button*/, NAS2D::Point<int> position)
{
	hasFocus(mRect.contains(position)); // This is a very useful check, should probably include this in all controls.

	if (!visible() || !enabled() || !hasFocus()) { return; }
	if (!editable()) { return; }

	const int virtualOffsetX = mScrollOffsetPixelX + position.x - (mRect.position.x + fieldPadding);

	// If the click occured past the width of the text, we can immediatly
	// set the position to the end and move on.
	if (virtualOffsetX > mFont.width(mText))
	{
		mCursorCharacterIndex = mText.length();
		updateScrollPosition();
		return;
	}


	// Figure out where the click occured within the visible string.
	for (std::size_t subStringLength = 1; subStringLength <= mText.length(); ++subStringLength)
	{
		const int subStringSizeX = mFont.width(mText.substr(0, subStringLength));
		if (subStringSizeX > virtualOffsetX)
		{
			mCursorCharacterIndex = subStringLength - 1;
			updateScrollPosition();
			break;
		}
	}
}


void TextField::onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier mod, bool /*repeat*/)
{
	if (!visible() || !enabled() || !hasFocus()) { return; }
	if (!editable()) { return; }

	switch(key)
	{
		// Command keys
		case NAS2D::KeyCode::Backspace:
			if (!mText.empty() && mCursorCharacterIndex > 0)
			{
				mCursorCharacterIndex--;
				mText.erase(mCursorCharacterIndex, 1);
				updateScrollPosition();
				onTextChange();
			}
			break;

		case NAS2D::KeyCode::Delete:
			if (!mText.empty())
			{
				mText = mText.erase(mCursorCharacterIndex, 1);
				updateScrollPosition();
				onTextChange();
			}
			break;

		case NAS2D::KeyCode::Home:
			mCursorCharacterIndex = 0;
			updateScrollPosition();
			break;

		case NAS2D::KeyCode::End:
			mCursorCharacterIndex = mText.length();
			updateScrollPosition();
			break;

		// Arrow keys
		case NAS2D::KeyCode::Left:
			if (mCursorCharacterIndex > 0)
			{
				--mCursorCharacterIndex;
				updateScrollPosition();
			}
			break;

		case NAS2D::KeyCode::Right:
			if (mCursorCharacterIndex < mText.length())
			{
				++mCursorCharacterIndex;
				updateScrollPosition();
			}
			break;

		// Keypad arrow keys
		case NAS2D::KeyCode::Keypad4:
			if ((mCursorCharacterIndex > 0) && !NAS2D::EventHandler::numlock(mod))
			{
				--mCursorCharacterIndex;
				updateScrollPosition();
			}
			break;

		case NAS2D::KeyCode::Keypad6:
			if ((mCursorCharacterIndex < mText.length()) && !NAS2D::EventHandler::numlock(mod))
			{
				++mCursorCharacterIndex;
				updateScrollPosition();
			}
			break;

		// Enter/Return (ignore)
		case NAS2D::KeyCode::Enter:
		case NAS2D::KeyCode::KeypadEnter:
			break;

		// Regular keys
		default:
			break;
	}
}


void TextField::onTextInput(const std::string& newTextInput)
{
	if (!visible() || !enabled() || !hasFocus()) { return; }
	if (!editable() || newTextInput.empty()) { return; }
	if (mMaxCharacters > 0 && mText.length() >= mMaxCharacters) { return; }
	if (mNumbersOnly && !std::isdigit(newTextInput[0], std::locale{})) { return; }

	mText.insert(mCursorCharacterIndex, newTextInput);
	mCursorCharacterIndex += newTextInput.length();
	updateScrollPosition();
	onTextChange();
}


void TextField::onTextChange()
{
	if (mTextChangedHandler) { mTextChangedHandler(*this); }
}
