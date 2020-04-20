// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ==================================================================================
// = NAS2D+GUI
// = Copyright © 2008 - 2020, Leeor Dicker
// ==================================================================================
// = This file is part of the NAS2D+GUI library.
// ==================================================================================

#include "TextField.h"

#include "../../Constants.h"
#include "../../FontManager.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Renderer/Renderer.h"
#include "NAS2D/MathUtils.h"

#include <locale>

using namespace NAS2D;


static const int FIELD_PADDING = 4;

static const int CURSOR_BLINK_DELAY = 250;

static std::locale LOC;

static Font* TXT_FONT = nullptr;


TextField::TextField()
{
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &TextField::onMouseDown);
	Utility<EventHandler>::get().keyDown().connect(this, &TextField::onKeyDown);
	Utility<EventHandler>::get().textInput().connect(this, &TextField::onTextInput);

	hasFocus(true);
	Utility<EventHandler>::get().textInputMode(true);

	mSkinNormal.push_back(Image("ui/skin/textbox_top_left.png"));
	mSkinNormal.push_back(Image("ui/skin/textbox_top_middle.png"));
	mSkinNormal.push_back(Image("ui/skin/textbox_top_right.png"));
	mSkinNormal.push_back(Image("ui/skin/textbox_middle_left.png"));
	mSkinNormal.push_back(Image("ui/skin/textbox_middle_middle.png"));
	mSkinNormal.push_back(Image("ui/skin/textbox_middle_right.png"));
	mSkinNormal.push_back(Image("ui/skin/textbox_bottom_left.png"));
	mSkinNormal.push_back(Image("ui/skin/textbox_bottom_middle.png"));
	mSkinNormal.push_back(Image("ui/skin/textbox_bottom_right.png"));

	mSkinFocus.push_back(Image("ui/skin/textbox_top_left_highlight.png"));
	mSkinFocus.push_back(Image("ui/skin/textbox_top_middle_highlight.png"));
	mSkinFocus.push_back(Image("ui/skin/textbox_top_right_highlight.png"));
	mSkinFocus.push_back(Image("ui/skin/textbox_middle_left_highlight.png"));
	mSkinFocus.push_back(Image("ui/skin/textbox_middle_middle_highlight.png"));
	mSkinFocus.push_back(Image("ui/skin/textbox_middle_right_highlight.png"));
	mSkinFocus.push_back(Image("ui/skin/textbox_bottom_left_highlight.png"));
	mSkinFocus.push_back(Image("ui/skin/textbox_bottom_middle_highlight.png"));
	mSkinFocus.push_back(Image("ui/skin/textbox_bottom_right_highlight.png"));

	TXT_FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	height(static_cast<float>(TXT_FONT->height() + FIELD_PADDING * 2));
}


TextField::~TextField()
{
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &TextField::onMouseDown);
	Utility<EventHandler>::get().keyDown().disconnect(this, &TextField::onKeyDown);
	Utility<EventHandler>::get().textInput().disconnect(this, &TextField::onTextInput);
}


void TextField::resetCursorPosition()
{
	mCursorPosition = 0;
}


/**
 * When set, will only allow numbers to be entered into the TextField.
 * 
 * \param _b True or False.
 */
void TextField::numbers_only(bool _b)
{
	mNumbersOnly = _b;
}


/**
 * Sets the maximum number of characters allowed in the text field.
 * 
 * \param	Count	Number of characters allowed in the field.
 * 
 * \note	Calling this with \c 0 will clear character limit.
 */
void TextField::maxCharacters(std::size_t count)
{
	mMaxCharacters = count;
}


int TextField::textAreaWidth() const
{
	return static_cast<int>(rect().width()) - FIELD_PADDING * 2;
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
void TextField::onTextInput(const std::string& _s)
{
	if (!hasFocus() || !visible() || !editable() || _s.empty()) { return; }

	if (mMaxCharacters > 0 && text().length() == mMaxCharacters) { return; }

	auto prvLen = text().length();

	if (mNumbersOnly && !std::isdigit(_s[0], LOC)) { return; }

	mText = mText.insert(mCursorPosition, _s);

	if (text().length() - prvLen != 0u)
	{
		onTextChanged();
		mCursorPosition++;
	}
}


void TextField::onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier /*mod*/, bool /*repeat*/)
{
	if (!hasFocus() || !editable() || !visible()) { return; }

	switch(key)
	{
		// COMMAND KEYS
		case EventHandler::KeyCode::KEY_BACKSPACE:
			if(!text().empty() && mCursorPosition > 0)
			{
				mCursorPosition--;
				mText.erase(mCursorPosition, 1);
				onTextChanged();
			}
			break;

		case EventHandler::KeyCode::KEY_HOME:
			mCursorPosition = 0;
			break;

		case EventHandler::KeyCode::KEY_END:
			mCursorPosition = static_cast<int>(text().length());
			break;

		case EventHandler::KeyCode::KEY_DELETE:
			if (text().length() > 0)
			{
				mText = mText.erase(mCursorPosition, 1);
				onTextChanged();
			}
			break;

		// ARROW KEYS
		case EventHandler::KeyCode::KEY_LEFT:
			if(mCursorPosition > 0)
				--mCursorPosition;
			break;

		case EventHandler::KeyCode::KEY_RIGHT:
			if(static_cast<std::size_t>(mCursorPosition) < text().length())
				++mCursorPosition;
			break;

		// KEYPAD ARROWS
		case EventHandler::KeyCode::KEY_KP4:
			if((mCursorPosition > 0) && !Utility<EventHandler>::get().query_numlock())
				--mCursorPosition;
			break;

		case EventHandler::KeyCode::KEY_KP6:
			if((static_cast<std::size_t>(mCursorPosition) < text().length()) && !Utility<EventHandler>::get().query_numlock())
				++mCursorPosition;
			break;

		// IGNORE ENTER/RETURN KEY
		case EventHandler::KeyCode::KEY_ENTER:
		case EventHandler::KeyCode::KEY_KP_ENTER:
			break;

		// REGULAR KEYS
		default:
			break;
	}
}


/**
 * Mouse down even handler.
 */
void TextField::onMouseDown(EventHandler::MouseButton /*button*/, int x, int y)
{
	hasFocus(rect().contains(Point{x, y})); // This is a very useful check, should probably include this in all controls.

	if (!enabled() || !visible() || !hasFocus()) { return; }

	int relativePosition = static_cast<int>(x - rect().x());

	// If the click occured past the width of the text, we can immediatly
	// set the position to the end and move on.
	if(TXT_FONT->width(text()) < relativePosition)
	{
		mCursorPosition = static_cast<int>(text().size());
		return;
	}


	// Figure out where the click occured within the visible string.
	int i = 0;
	while(static_cast<std::size_t>(i) <= text().size() - mScrollOffset)
	{
		std::string cmpStr = text().substr(mScrollOffset, i);
		int strLen = TXT_FONT->width(cmpStr);
		if(strLen > relativePosition)
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
void TextField::drawCursor()
{
	if(hasFocus() && editable())
	{
		if(mShowCursor)
		{
			// updateCursor() should be called only on events relating to the cursor so this is temporary.
			updateCursor();
			Utility<Renderer>::get().drawLine(static_cast<float>(mCursorX + 1), rect().y() + FIELD_PADDING + 1, static_cast<float>(mCursorX + 1), rect().y() + rect().height() - FIELD_PADDING, 0, 0, 0);
			Utility<Renderer>::get().drawLine(static_cast<float>(mCursorX), rect().y() + FIELD_PADDING, static_cast<float>(mCursorX), rect().y() + rect().height() - FIELD_PADDING - 1, 255, 255, 255);
		}
		
		if(mCursorTimer.accumulator() > CURSOR_BLINK_DELAY)
		{
			mCursorTimer.reset();
			mShowCursor = !mShowCursor;
		}
	}
}


/**
 * Draws a highlight over selected text.
 */
void TextField::drawTextHighlight()
{
	Utility<Renderer>::get().drawBoxFilled(rect().x() + FIELD_PADDING, rect().y(), static_cast<float>(TXT_FONT->width(text())), rect().height(), 0, 0, 150, 100);
}


void TextField::updateCursor()
{
	int cursorX = TXT_FONT->width(text().substr(0, mCursorPosition));

	if (cursorX - mScrollOffset >= textAreaWidth())
	{
		mScrollOffset = cursorX - textAreaWidth();
	}

	if (cursorX - mScrollOffset <= 0)
	{
		mScrollOffset = cursorX - textAreaWidth() / 2;
	}

	if (mScrollOffset < 0)
	{
		mScrollOffset = 0;
	}

	mCursorX = static_cast<int>(rect().x() + FIELD_PADDING + cursorX - mScrollOffset);
}


void TextField::update()
{
	if (!visible()) { return; }

	Renderer& r = Utility<Renderer>::get();

	if (hasFocus() && editable()) { r.drawImageRect(rect().x(), rect().y(), rect().width(), rect().height(), mSkinFocus); }
	else { r.drawImageRect(rect().x(), rect().y(), rect().width(), rect().height(), mSkinNormal); }

	if (highlight()) { r.drawBox(rect(), 255, 255, 0); }

	drawCursor();

	r.drawText(*TXT_FONT, text(), positionX() + FIELD_PADDING, positionY() + FIELD_PADDING, 255, 255, 255);
}
