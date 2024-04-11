#include "KeyboardInputHandler.h"

#include <NAS2D/Math/Vector.h>
#include <OPHD/Map/MapOffset.h>
#include <OPHD/DirectionOffset.h>
#include <OPHD/States/MapViewState.h>


void KeyboardInputHandler::handleInput(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier keyModifiers)
{
	auto keyModifiersFitered = NAS2D::EventHandler::KeyModifier::None;

	mMoveCommandScalar = 1;

	NAS2D::Utility<NAS2D::EventHandler>::get().shift(keyModifiers) ? keyModifiersFitered |= NAS2D::EventHandler::KeyModifier::Shift : keyModifiersFitered;

	NAS2D::Utility<NAS2D::EventHandler>::get().control(keyModifiers) ? keyModifiersFitered |= NAS2D::EventHandler::KeyModifier::Ctrl : keyModifiersFitered;

	if(mModifierCommandMap.find(keyModifiersFitered) != mModifierCommandMap.end())
	{
		mModifierCommandMap[keyModifiersFitered]->execute();
	}

	if (mKeyCodeMap.find(keyModifiersFitered) != mKeyCodeMap.end() && mKeyCodeMap[keyModifiersFitered].find(key) != mKeyCodeMap[keyModifiersFitered].end())
	{
		mKeyCodeMap[keyModifiersFitered][key]->execute();
	}
	else if (mKeyCodeMap[NAS2D::EventHandler::KeyModifier::None].find(key) != mKeyCodeMap[NAS2D::EventHandler::KeyModifier::None].end())
	{
		mKeyCodeMap[NAS2D::EventHandler::KeyModifier::None][key]->execute();
	}
}
