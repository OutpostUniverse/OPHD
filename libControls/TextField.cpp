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


namespace
{
	constexpr int fieldPadding = 4;
	constexpr int cursorBlinkDelay = 250;
}


TextField::TextField(std::size_t maxCharacters, TextChangedDelegate textChangedHandler) :
	mFont{getDefaultFont()},
	mSkinNormal{loadRectangleSkin("ui/skin/textbox_normal")},
	mSkinFocus{loadRectangleSkin("ui/skin/textbox_highlight")},
	mTextChangedHandler{textChangedHandler},
	mMaxCharacters{maxCharacters}
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
	mText = text;
	onTextChange();
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
	mText.clear();
	mCursorCharacterIndex = 0;
	onTextChange();
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

	// Should be called only on events relating to the cursor so this is temporary.
	updateScrollPosition();

	if (mCursorBlinkTimer.elapsedTicks() > cursorBlinkDelay)
	{
		mCursorBlinkTimer.reset();
		mShowCursor = !mShowCursor;
	}

	draw();
}


void TextField::updateScrollPosition()
{
	int cursorX = mFont.width(mText.substr(0, mCursorCharacterIndex));

	// Check if cursor is after visible area
	if (mScrollOffsetPixelX <= cursorX - textAreaWidth())
	{
		mScrollOffsetPixelX = cursorX - textAreaWidth();
	}

	// Check if cursor is before visible area
	if (mScrollOffsetPixelX >= cursorX)
	{
		mScrollOffsetPixelX = cursorX - textAreaWidth() / 2;
	}

	if (mScrollOffsetPixelX < 0)
	{
		mScrollOffsetPixelX = 0;
	}

	mCursorPixelX = mRect.position.x + fieldPadding + cursorX - mScrollOffsetPixelX;
}


int TextField::textAreaWidth() const
{
	return mRect.size.x - fieldPadding * 2;
}


void TextField::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	const auto showFocused = hasFocus() && editable();
	const auto& skin = showFocused ? mSkinFocus : mSkinNormal;
	skin.draw(renderer, mRect);

	if (highlight()) { renderer.drawBox(mRect, NAS2D::Color::Yellow); }

	drawCursor();

	const auto textDrawArea = mRect.inset(fieldPadding);
	renderer.clipRect(textDrawArea);
	renderer.drawText(mFont, mText, textDrawArea.position + NAS2D::Vector{-mScrollOffsetPixelX, 0}, NAS2D::Color::White);
	renderer.clipRectClear();
}


void TextField::drawCursor() const
{
	if (hasFocus() && editable())
	{
		if (mShowCursor)
		{
			auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
			const auto startPosition = NAS2D::Point{mCursorPixelX, mRect.position.y + fieldPadding};
			const auto endPosition = NAS2D::Point{mCursorPixelX, mRect.position.y + mRect.size.y - fieldPadding - 1};
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
		return;
	}


	// Figure out where the click occured within the visible string.
	for (std::size_t index = 0; index < mText.length(); ++index)
	{
		const int subStringSizeX = mFont.width(mText.substr(0, index));
		if (subStringSizeX > virtualOffsetX)
		{
			mCursorCharacterIndex = index - 1;
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
				onTextChange();
			}
			break;

		case NAS2D::KeyCode::Delete:
			if (!mText.empty())
			{
				mText = mText.erase(mCursorCharacterIndex, 1);
				onTextChange();
			}
			break;

		case NAS2D::KeyCode::Home:
			mCursorCharacterIndex = 0;
			break;

		case NAS2D::KeyCode::End:
			mCursorCharacterIndex = mText.length();
			break;

		// Arrow keys
		case NAS2D::KeyCode::Left:
			if (mCursorCharacterIndex > 0)
				--mCursorCharacterIndex;
			break;

		case NAS2D::KeyCode::Right:
			if (mCursorCharacterIndex < mText.length())
				++mCursorCharacterIndex;
			break;

		// Keypad arrow keys
		case NAS2D::KeyCode::Keypad4:
			if ((mCursorCharacterIndex > 0) && !NAS2D::EventHandler::numlock(mod))
				--mCursorCharacterIndex;
			break;

		case NAS2D::KeyCode::Keypad6:
			if ((mCursorCharacterIndex < mText.length()) && !NAS2D::EventHandler::numlock(mod))
				++mCursorCharacterIndex;
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
	onTextChange();
	mCursorCharacterIndex++;
}


void TextField::onTextChange()
{
	if (mTextChangedHandler) { mTextChangedHandler(*this); }
}
