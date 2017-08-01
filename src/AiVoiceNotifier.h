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
	void gender(AiGender _g) { mGender = _g; }

protected:

private:

	typedef std::map<AiMessage, NAS2D::Sound*> MessageSoundTable; 

private:

	// No default constructor, copy constructor, or copy operator
	// Calling these should result in an error
	AiVoiceNotifier() = delete;
	AiVoiceNotifier(const AiVoiceNotifier&) = delete;
	AiVoiceNotifier& operator=(const AiVoiceNotifier&) = delete;

	void buildVoiceTables();

private:

	AiGender				mGender;

	std::map<AiGender, MessageSoundTable>	mVoiceTable;
};