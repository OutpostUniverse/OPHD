#pragma once
#include <unordered_map>

#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Signal/Signal.h>

#include "../Constants/Strings.h"
#include "../Map/MapView.h"
#include "../DirectionOffset.h"

#include "Command.h"
#include "Commands.cpp"

class KeyboardInputHandler
{
public:
	KeyboardInputHandler(){};
	void handleInput(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier keyModifiers);

	//Register a command that will be executed when a key or combination of key and key modifiers are pressed
	void registerCommand(NAS2D::EventHandler::KeyModifier keyModifier, NAS2D::EventHandler::KeyCode keyCode, Command* command)
	{
		mKeyCodeMap[keyModifier][keyCode] = command;
	}

	//Register a move command that will be executed when a key or combination of key and key modifiers are pressed
	void registerCommand(NAS2D::EventHandler::KeyModifier keyModifier, NAS2D::EventHandler::KeyCode keyCode, MoveCommand* command)
	{
		command->setMoveCommandScalar_(&mMoveCommandScalar);
		mKeyCodeMap[keyModifier][keyCode] = command;
	}

	//Register a command that will be executed when a modifier key of combination of (ie. Ctrl, Shift, Alt, Meta) is pressed
	void registerCommand(NAS2D::EventHandler::KeyModifier keyModifier, ModifierCommand* command)
	{
		command->setModifier_(modifier_);
		mModifierCommandMap[keyModifier] = command;
	}

private:
	int mMoveCommandScalar = 1;

	std::unordered_map<NAS2D::EventHandler::KeyModifier, Command*> mModifierCommandMap;
	std::unordered_map<NAS2D::EventHandler::KeyModifier, std::unordered_map<NAS2D::EventHandler::KeyCode, Command*>> mKeyCodeMap;

	Command* mNullCommand = new NullCommand();
};
