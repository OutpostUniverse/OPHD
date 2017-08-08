#pragma once

#include <array>

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
		LANDER_NO_BULLDOZE,
		LOCATION_NOT_BULLDOZED,
		MAX_DIGGING_DEPTH_REACHED,
		NO_TUBE_CONNECTION,
		OUT_OF_COMM_RANGE,
		UNSUITABLE_LANDING_SITE,
		TILE_BULLDOZED,
		NOT_IMPLEMENTED
	};

	enum AiGender
	{
		MALE,
		FEMALE
	};

public:
	AiVoiceNotifier();
	AiVoiceNotifier(AiGender _g);
	~AiVoiceNotifier();

	void notify(AiMessage _msg);

	AiGender gender() const { return mGender; }
	void gender(AiGender _g) { mGender = _g; }

private:
	typedef std::array<NAS2D::Sound*, NOT_IMPLEMENTED + 1> MessageSoundTable;


private:

	// No default constructor, copy constructor, or copy operator
	// Calling these should result in an error
	AiVoiceNotifier(const AiVoiceNotifier&) = delete;
	AiVoiceNotifier& operator=(const AiVoiceNotifier&) = delete;

	void buildVoiceTables();

private:

	AiGender								mGender = MALE;

	std::map<AiGender, MessageSoundTable>	mVoiceTable;
};