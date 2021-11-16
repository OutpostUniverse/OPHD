#include "Technology.h"

#include <stdexcept>


Technology::Effect::Effect(Type type) :
	type(type)
{}


bool Technology::Effect::isModifier() const
{
	return type == Type::Modifier;
}


bool Technology::Effect::isUnlock() const
{
	return type == Type::Unlock;
}


Technology::Modifier& Technology::Effect::toModifier()
{
	if (!isModifier())
	{
		throw std::runtime_error("Effect::toModifier(): Requesting a Modifier cast when effect is not a Modifier.");
	}
	return static_cast<Modifier&>(*this);
}


Technology::Unlock& Technology::Effect::toUnlock()
{
	if (!isUnlock())
	{
		throw std::runtime_error("Effect::toUnlock(): Requesting an Unlock cast when effect is not an Unlock.");
	}
	return static_cast<Unlock&>(*this);
}


Technology::Modifier::Modifier(Modifies modifies, float value) :
	Effect(Type::Modifier),
	modifies(modifies),
	value(value)
{}


Technology::Unlock::Unlock(Unlocks unlocks, std::string value) :
	Effect(Type::Unlock),
	unlocks(unlocks),
	value(value)
{}
