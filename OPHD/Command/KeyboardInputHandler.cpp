#include "KeyboardInputHandler.h"

#include <NAS2D/Math/Vector.h>
#include <OPHD/Map/MapOffset.h>
#include <OPHD/DirectionOffset.h>
#include <OPHD/States/MapViewState.h>


void KeyboardInputHandler::handleInput(NAS2D::EventHandler::KeyModifier keyModifiers, NAS2D::EventHandler::KeyCode keyCode)
{
	auto keyModifiersFiltered = NAS2D::EventHandler::KeyModifier::None;

	mMoveCommandScalar = 1;

	NAS2D::Utility<NAS2D::EventHandler>::get().shift(keyModifiers) ? keyModifiersFiltered |= NAS2D::EventHandler::KeyModifier::Shift : keyModifiersFiltered;

	NAS2D::Utility<NAS2D::EventHandler>::get().control(keyModifiers) ? keyModifiersFiltered |= NAS2D::EventHandler::KeyModifier::Ctrl : keyModifiersFiltered;

	if(mModifierCommandMap.find(keyModifiersFiltered) != mModifierCommandMap.end())
	{
		mModifierCommandMap[keyModifiersFiltered]->execute();
	}

	if (mKeyCodeMap.find(keyModifiersFiltered) != mKeyCodeMap.end() && mKeyCodeMap[keyModifiersFiltered].find(key) != mKeyCodeMap[keyModifiersFiltered].end())
	{
		mKeyCodeMap[keyModifiersFiltered][key]->execute();
	}
	else if (mKeyCodeMap[NAS2D::EventHandler::KeyModifier::None].find(key) != mKeyCodeMap[NAS2D::EventHandler::KeyModifier::None].end())
	{
		mKeyCodeMap[NAS2D::EventHandler::KeyModifier::None][key]->execute();
	}
}
