// ==================================================================================
// = NAS2D+GUI
// = Copyright © 2008 - 2012, New Age Software
// ==================================================================================
// = This file is part of the NAS2D+GUI library.
// ==================================================================================

#include "TextField.h"

#include "NAS2D/KeyTranslator.h"


const int MIN_WIDTH		= 16;
const int MIN_HEIGHT	= 16;

const int MAX_WIDTH		= 512;
const int MAX_HEIGHT	= 32;

const int REPEAT_DELAY	= 500;
const int REPEAT_WAIT	= 20;


const int FIELD_PADDING = 4;


const int CURSOR_BLINK_DELAY = 250;

TextField::TextField():	mCursorPosition(0),
						mCursorX(0),
						mScrollOffset(0),
						mMaxScrollOffset(0),
						mLastAction(ACTION_NONE),
						mBorderVisibility(FOCUS_ONLY),
						mEditable(true),
						mShowCursor(false)
{
	Utility<EventHandler>::get().mouseButtonDown().Connect(this, &TextField::onMouseDown);
	Utility<EventHandler>::get().keyDown().Connect(this, &TextField::onKeyDown);
	Utility<EventHandler>::get().keyUp().Connect(this, &TextField::onKeyUp);
}


TextField::~TextField()
{
}


void TextField::onTextChanged()
{
	mCursorPosition = 0;
}


void TextField::onFontChanged()
{
	height(font().height() + FIELD_PADDING * 2);
}


int TextField::textAreaWidth() const
{
	return rect().w() - FIELD_PADDING * 2;
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


void TextField::draw()
{
	Renderer& r = Utility<Renderer>::get();

	if((mBorderVisibility == ALWAYS) || (hasFocus() && mBorderVisibility != NEVER))
	{
		r.drawBoxFilled(rect().x(), rect().y(), rect().w(), rect().h(), 0, 0, 0, 65);
		r.drawBox(rect().x() + 1, rect().y() + 1, rect().w(), rect().h(), 0, 0, 0);
		r.drawBox(rect(), 255, 255, 255);
	}

	if(highlight())
		r.drawBox(rect(), 255, 255, 0);

	drawCursor();

	if (fontSet())
	{
		r.drawText(font(), text(), positionX() + FIELD_PADDING, positionY() + FIELD_PADDING, 255, 255, 255);
	}
}


/**
 * Draws the insertion point cursor.
 */
void TextField::drawCursor()
{
	if(hasFocus())
	{
		if(mShowCursor)
		{
			// updateCursor() should be called only on events relating to the cursor so this is temporary.
			updateCursor();
			Utility<Renderer>::get().drawLine(mCursorX + 1, rect().y() + FIELD_PADDING + 1, mCursorX + 1, rect().y() + rect().h() - FIELD_PADDING, 0, 0, 0);
			Utility<Renderer>::get().drawLine(mCursorX, rect().y() + FIELD_PADDING, mCursorX, rect().y() + rect().h() - FIELD_PADDING - 1, 255, 255, 255);
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
	Utility<Renderer>::get().drawBoxFilled(rect().x() + FIELD_PADDING, rect().y(), font().width(text()), rect().h(), 0, 0, 150, 100);
}


void TextField::updateCursor()
{
	int cursorX = font().width(text().substr(0, mCursorPosition));

	if(cursorX - mScrollOffset >= textAreaWidth())
		mScrollOffset = cursorX - textAreaWidth();
	if(cursorX - mScrollOffset <= 0)
		mScrollOffset = cursorX - textAreaWidth() / 2;

	if(mScrollOffset < 0)
		mScrollOffset = 0;


	mCursorX = rect().x() + FIELD_PADDING + cursorX - mScrollOffset;
}


void TextField::update()
{
	if(!visible())
		return;

	draw();

	if((mLastAction == ACTION_NONE) || (mActionTimer.accumulator() < REPEAT_DELAY))
		return;

	if(mRepeatTimer.accumulator() > REPEAT_WAIT)
	{
		mRepeatTimer.reset();
		switch(mLastAction)
		{
			case ACTION_INSERT:
				addCharacter(mInsertChar.c_str());
				break;

			case ACTION_BACKSPACE:
				if(!text().empty() && mCursorPosition > 0)
				{
					mCursorPosition--;
					_text().erase(mCursorPosition, 1);
				}
				break;

			case ACTION_DELETE:
				if(text().length() > 0)
					_text() = _text().erase(mCursorPosition, 1);
				break;

			case ACTION_MOVE_LEFT:
				if(mCursorPosition > 0)
					mCursorPosition--;
				break;

			case ACTION_MOVE_RIGHT:
				if(static_cast<size_t>(mCursorPosition) < text().length())
					mCursorPosition++;
				break;

			default:
				break;
		}
	}
}


void TextField::onKeyDown(KeyCode key, KeyModifier mod, bool repeat)
{
	if(hasFocus() && editable())
	{
		switch(key)
		{	
			// COMMAND KEYS
			case KEY_BACKSPACE:
				if(!text().empty() && mCursorPosition > 0)
				{
					mCursorPosition--;
					// STL '.end()' iterators are the end of the container, not the last element.
					_text().erase(mCursorPosition, 1);
					setAction(ACTION_BACKSPACE);
				}
				break;

			case KEY_HOME:
				mCursorPosition = 0;
				break;

			case KEY_END:
				mCursorPosition = text().length();
				break;

			case KEY_DELETE:
				if(text().length() > 0)
				{
					_text() = _text().erase(mCursorPosition, 1);
					setAction(ACTION_DELETE);
				}
				break;

			// ARROW KEYS
			case KEY_LEFT:
				if(mCursorPosition > 0)
				{
					mCursorPosition--;
					setAction(ACTION_MOVE_LEFT);
				}
				break;

			case KEY_RIGHT:
				if(static_cast<size_t>(mCursorPosition) < text().length())
				{
					mCursorPosition++;
					setAction(ACTION_MOVE_RIGHT);
				}
				break;

			// KEYPAD ARROWS
			case KEY_KP4:
				if((mCursorPosition > 0) && !KeyTranslator::numlock(mod))
				{
					mCursorPosition--;
					setAction(ACTION_MOVE_LEFT);
				}
				else
					insertChar(key, mod);
				break;

			case KEY_KP6:
				if((static_cast<size_t>(mCursorPosition) < text().length()) && !KeyTranslator::numlock(mod))
				{
					mCursorPosition++;
					setAction(ACTION_MOVE_RIGHT);
				}
				else
					insertChar(key, mod);
				break;

			// IGNORE ENTER/RETURN KEY
			case KEY_ENTER:
			case KEY_KP_ENTER:
				break;

			// REGULAR KEYS
			default:
				insertChar(key, mod);
				break;
		}
	}
}


void TextField::onKeyUp(KeyCode key, KeyModifier mod)
{
	mLastAction = ACTION_NONE;
}


void TextField::insertChar(KeyCode key, KeyModifier mod)
{
	mInsertChar = KeyTranslator::translate(key, mod);
	addCharacter(mInsertChar.c_str());
	setAction(ACTION_INSERT);
}


/**
 * Adds a character to the TextField's text.
 */
void TextField::addCharacter(const char* character)
{
	int prvLen = text().length();
	_text() = _text().insert(mCursorPosition, character);

	if(text().length() - prvLen)
		mCursorPosition++;
}


/**
 * Set the current action and reset the action timer.
 */
void TextField::setAction(ActionType aType)
{
	mLastAction = aType;
	mActionTimer.reset();
}


/**
 * Mouse down even handler.
 */
void TextField::onMouseDown(MouseButton button, int x, int y)
{
	// If font is not available, back out now to prevent issues.
	if(!fontSet())
		return;

	if(!isPointInRect(Point_2d(x, y), rect()))
	{
		hasFocus(false);
		return;
	}
	else
		hasFocus(true);

	int relativePosition = x - rect().x();

	// If the click occured past the width of the text, we can immediatly
	// set the position to the end and move on.
	if(font().width(text()) < relativePosition)
	{
		mCursorPosition = text().size();
		return;
	}


	// Figure out where the click occured within the visible string.
	int i = 0;
	while(static_cast<size_t>(i) <= text().size() - mScrollOffset)
	{
		string cmpStr = text().substr(mScrollOffset, i);
		int strLen = font().width(cmpStr);
		if(strLen > relativePosition)
		{
			mCursorPosition = i - 1;
			break;
		}

		i++;
	}
}