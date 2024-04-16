#include "KeyboardInputHandler.h"

#include <NAS2D/Math/Vector.h>
#include <OPHD/Map/MapOffset.h>
#include <OPHD/DirectionOffset.h>
#include <OPHD/States/MapViewState.h>


void KeyboardInputHandler::handleInput(NAS2D::EventHandler::KeyModifier keyModifiers, NAS2D::EventHandler::KeyCode keyCode)
{
	auto keyModifiersMask = NAS2D::EventHandler::KeyModifier::Ctrl | NAS2D::EventHandler::KeyModifier::Shift | NAS2D::EventHandler::KeyModifier::Alt;
	auto keyModifiersFiltered = keyModifiers & keyModifiersMask;

	mMoveCommandScalar = 1;

	modifierCommand(keyModifiersFiltered)->execute();
	keyCodeCommand(keyModifiersFiltered, keyCode)->execute();
}


Command* KeyboardInputHandler::modifierCommand(NAS2D::EventHandler::KeyModifier keyModifier)
{
	auto keyModifierIterator = mModifierCommandMap.find(keyModifier);
	if (keyModifierIterator != mModifierCommandMap.end())
	{
		return keyModifierIterator->second;
	}
	return mNullCommand;
}


Command* KeyboardInputHandler::keyCodeCommand(NAS2D::EventHandler::KeyModifier keyModifier, NAS2D::EventHandler::KeyCode keyCode)
{
	auto keyModifierIterator = mKeyCodeMap.find(keyModifier);
	if (keyModifierIterator != mKeyCodeMap.end())
	{
		auto keyCodeIterator = keyModifierIterator->second.find(keyCode);
		if (keyCodeIterator != keyModifierIterator->second.end())
		{
			return keyCodeIterator->second;
		}
	}

	keyModifierIterator = mKeyCodeMap.find(NAS2D::EventHandler::KeyModifier::None);
	if (keyModifierIterator != mKeyCodeMap.end())
	{
		auto keyCodeIterator = keyModifierIterator->second.find(keyCode);
		if (keyCodeIterator != keyModifierIterator->second.end())
		{
			return keyCodeIterator->second;
		}
	}

	return mNullCommand;
}
