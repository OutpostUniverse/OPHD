#pragma once

#include <map>

#include "NAS2D/NAS2D.h"



class AiVoiceNotifier
{
public:

	enum AiMessage
	{
		CC_NO_BULLDOZE,
		INSUFFICIENT_RESOURCES,
		INVALID_DIGGER_PLACEMENT,
		INVALID_MINER_PLACEMENT,
		INVALID_STRUCTURE_PLACEMENT,
		INVALID_TUBE_PLACEMENT,
		LOCATION_NOT_BULLDOZED,
		MAX_DIGGING_DEPTH_REACHED,
		NO_TUBE_CONNECTION,
		UNSUITABLE_LANDING_SITE
	};

	enum AiGender
	{
		MALE,
		FEMALE
	};

public:

	AiVoiceNotifier(AiGender _g);
	~AiVoiceNotifier();

	void notify(AiMessage _msg);

	AiGender gender() const { return mGender; }

protected:

private:

	typedef std::map<AiMessage, NAS2D::Sound*> MessageSoundTable; 

private:

	AiVoiceNotifier() {}									// Explicitely Declared Private
	AiVoiceNotifier(const AiVoiceNotifier&) {}				// Explicitely Declared Private
	AiVoiceNotifier& operator=(const AiVoiceNotifier&) {}	// Explicitely Declared Private

	void buildVoiceTables();

private:

	AiGender				mGender;

	std::map<AiGender, MessageSoundTable>	mVoiceTable;
};