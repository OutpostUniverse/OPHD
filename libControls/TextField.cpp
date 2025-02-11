// ==================================================================================
// = NAS2D+GUI
// = Copyright © 2008 - 2020, Leeor Dicker
// ==================================================================================
// = This file is part of the NAS2D+GUI library.
// ==================================================================================

#include "TextField.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>

#include <locale>


namespace
{
	constexpr int fieldPadding = 4;
	constexpr int cursorBlinkDelay = 250;
}


TextField::TextField() :
	mFont{getDefaultFont()},
	mSkinNormal{
		getImage("ui/skin/textbox_top_left.png"),
		getImage("ui/skin/textbox_top_middle.png"),
		getImage("ui/skin/textbox_top_right.png"),
		getImage("ui/skin/textbox_middle_left.png"),
		getImage("ui/skin/textbox_middle_middle.png"),
		getImage("ui/skin/textbox_middle_right.png"),
		getImage("ui/skin/textbox_bottom_left.png"),
		getImage("ui/skin/textbox_bottom_middle.png"),
		getImage("ui/skin/textbox_bottom_right.png")
	},
	mSkinFocus{
		getImage("ui/skin/textbox_top_left_highlight.png"),
		getImage("ui/skin/textbox_top_middle_highlight.png"),
		getImage("ui/skin/textbox_top_right_highlight.png"),
		getImage("ui/skin/textbox_middle_left_highlight.png"),
		getImage("ui/skin/textbox_middle_middle_highlight.png"),
		getImage("ui/skin/textbox_middle_right_highlight.png"),
		getImage("ui/skin/textbox_bottom_left_highlight.png"),
		getImage("ui/skin/textbox_bottom_middle_highlight.png"),
		getImage("ui/skin/textbox_bottom_right_highlight.png")
	}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().connect({this, &TextField::onMouseDown});
	eventHandler.keyDown().connect({this, &TextField::onKeyDown});
	eventHandler.textInput().connect({this, &TextField::onTextInput});

	eventHandler.textInputMode(true);

	height(mFont.height() + fieldPadding * 2);
}


TextField::~TextField()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect({this, &TextField::onMouseDown});
	eventHandler.keyDown().disconnect({this, &TextField::onKeyDown});
	eventHandler.textInput().disconnect({this, &TextField::onTextInput});
}


void TextField::resetCursorPosition()
{
	mCursorPosition = 0;
}


/**
 * When set, will only allow numbers to be entered into the TextField.
 * 
 * \param isNumbersOnly True or False.
 */
void TextField::numbers_only(bool isNumbersOnly)
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


int TextField::textAreaWidth() const
{
	return mRect.size.x - fieldPadding * 2;
}


void TextField::editable(bool editable)
{
	mEditable = editable;
}


bool TextField::editable() const
{
	return mEditable;
}


/**
 * Sets border visibility.
 */
void TextField::border(BorderVisibility visibility)
{
	mBorderVisibility = visibility;
}


/**
 * Handles text input events.
 */
void TextField::onTextInput(const std::string& newTextInput)
{
	if (!hasFocus() || !visible() || !editable() || newTextInput.empty()) { return; }

	if (mMaxCharacters > 0 && text().length() == mMaxCharacters) { return; }

	auto prvLen = text().length();

	std::locale locale;
	if (mNumbersOnly && !std::isdigit(newTextInput[0], locale)) { return; }

	mText = mText.insert(mCursorPosition, newTextInput);

	if (text().length() - prvLen != 0u)
	{
		onTextChange();
		mCursorPosition++;
	}
}


void TextField::onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool /*repeat*/)
{
	if (!hasFocus() || !editable() || !visible()) { return; }

	switch(key)
	{
		// COMMAND KEYS
		case NAS2D::EventHandler::KeyCode::KEY_BACKSPACE:
			if (!text().empty() && mCursorPosition > 0)
			{
				mCursorPosition--;
				mText.erase(mCursorPosition, 1);
				onTextChange();
			}
			break;

		case NAS2D::EventHandler::KeyCode::KEY_HOME:
			mCursorPosition = 0;
			break;

		case NAS2D::EventHandler::KeyCode::KEY_END:
			mCursorPosition = text().length();
			break;

		case NAS2D::EventHandler::KeyCode::KEY_DELETE:
			if (text().length() > 0)
			{
				mText = mText.erase(mCursorPosition, 1);
				onTextChange();
			}
			break;

		// ARROW KEYS
		case NAS2D::EventHandler::KeyCode::KEY_LEFT:
			if (mCursorPosition > 0)
				--mCursorPosition;
			break;

		case NAS2D::EventHandler::KeyCode::KEY_RIGHT:
			if (mCursorPosition < text().length())
				++mCursorPosition;
			break;

		// KEYPAD ARROWS
		case NAS2D::EventHandler::KeyCode::KEY_KP4:
			if ((mCursorPosition > 0) && !NAS2D::EventHandler::numlock(mod))
				--mCursorPosition;
			break;

		case NAS2D::EventHandler::KeyCode::KEY_KP6:
			if ((mCursorPosition < text().length()) && !NAS2D::EventHandler::numlock(mod))
				++mCursorPosition;
			break;

		// IGNORE ENTER/RETURN KEY
		case NAS2D::EventHandler::KeyCode::KEY_ENTER:
		case NAS2D::EventHandler::KeyCode::KEY_KP_ENTER:
			break;

		// REGULAR KEYS
		default:
			break;
	}
}


void TextField::onMouseDown(NAS2D::EventHandler::MouseButton /*button*/, NAS2D::Point<int> position)
{
	hasFocus(mRect.contains(position)); // This is a very useful check, should probably include this in all controls.

	if (!enabled() || !visible()) { return; }

	int relativePosition = position.x - mRect.position.x;

	// If the click occured past the width of the text, we can immediatly
	// set the position to the end and move on.
	if (mFont.width(text()) < relativePosition)
	{
		mCursorPosition = text().size();
		return;
	}


	// Figure out where the click occured within the visible string.
	std::size_t i = 0;
	const auto scrollOffset = static_cast<std::size_t>(mScrollOffset);
	while(i <= text().size() - scrollOffset)
	{
		std::string cmpStr = text().substr(scrollOffset, i);
		int strLen = mFont.width(cmpStr);
		if (strLen > relativePosition)
		{
			mCursorPosition = i - 1;
			break;
		}

		i++;
	}
}


/**
 * Draws the insertion point cursor.
 */
void TextField::drawCursor() const
{
	if (hasFocus() && editable())
	{
		if (mShowCursor)
		{
			auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
			const auto startPosition = NAS2D::Point{mCursorX, mRect.position.y + fieldPadding};
			const auto endPosition = NAS2D::Point{mCursorX, mRect.position.y + mRect.size.y - fieldPadding - 1};
			renderer.drawLine(startPosition + NAS2D::Vector{1, 1}, endPosition + NAS2D::Vector{1, 1}, NAS2D::Color::Black);
			renderer.drawLine(startPosition, endPosition, NAS2D::Color::White);
		}
	}
}


void TextField::updateScrollPosition()
{
	int cursorX = mFont.width(text().substr(0, mCursorPosition));

	// Check if cursor is after visible area
	if (mScrollOffset <= cursorX - textAreaWidth())
	{
		mScrollOffset = cursorX - textAreaWidth();
	}

	// Check if cursor is before visible area
	if (mScrollOffset >= cursorX)
	{
		mScrollOffset = cursorX - textAreaWidth() / 2;
	}

	if (mScrollOffset < 0)
	{
		mScrollOffset = 0;
	}

	mCursorX = mRect.position.x + fieldPadding + cursorX - mScrollOffset;
}


void TextField::update()
{
	if (!visible()) { return; }

	// Should be called only on events relating to the cursor so this is temporary.
	updateScrollPosition();

	if (mCursorTimer.elapsedTicks() > cursorBlinkDelay)
	{
		mCursorTimer.reset();
		mShowCursor = !mShowCursor;
	}

	draw();
}


void TextField::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	const auto showFocused = hasFocus() && editable();
	const auto& skin = showFocused ? mSkinFocus : mSkinNormal;
	skin.draw(renderer, mRect);

	if (highlight()) { renderer.drawBox(mRect, NAS2D::Color::Yellow); }

	drawCursor();

	renderer.drawText(mFont, text(), position() + NAS2D::Vector{fieldPadding, fieldPadding}, NAS2D::Color::White);
}
