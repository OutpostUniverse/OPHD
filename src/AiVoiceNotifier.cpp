#include "AiVoiceNotifier.h"

using namespace NAS2D;

AiVoiceNotifier::AiVoiceNotifier(AiGender _g): mGender(_g)
{
	buildVoiceTables();
}


AiVoiceNotifier::~AiVoiceNotifier()
{
	for (auto voice_it = mVoiceTable.begin(); voice_it != mVoiceTable.end(); ++voice_it)
		for (auto it = voice_it->second.begin(); it != voice_it->second.end(); ++it)
			delete it->second;
}

void AiVoiceNotifier::notify(AiMessage _msg)
{
	Utility<Mixer>::get().playSound(*mVoiceTable[gender()][_msg]);
}


void AiVoiceNotifier::buildVoiceTables()
{
	mVoiceTable[MALE][CC_NO_BULLDOZE] = new Sound("sfx/cc_no_bulldoze_m.ogg");
	mVoiceTable[MALE][INSUFFICIENT_RESOURCES] = new Sound("sfx/insufficient_resources_m.ogg");
	mVoiceTable[MALE][INVALID_DIGGER_PLACEMENT] = new Sound("sfx/invalid_digger_placement_m.ogg");
	mVoiceTable[MALE][INVALID_MINER_PLACEMENT] = new Sound("sfx/invalid_miner_placement_m.ogg");
	mVoiceTable[MALE][INVALID_STRUCTURE_PLACEMENT] = new Sound("sfx/invalid_structure_placement_m.ogg");
	mVoiceTable[MALE][INVALID_TUBE_PLACEMENT] = new Sound("sfx/invalid_tube_placement_m.ogg");
	mVoiceTable[MALE][LOCATION_NOT_BULLDOZED] = new Sound("sfx/location_not_bulldozed_m.ogg");
	mVoiceTable[MALE][MAX_DIGGING_DEPTH_REACHED] = new Sound("sfx/max_dig_depth_m.ogg");
	mVoiceTable[MALE][NO_TUBE_CONNECTION] = new Sound("sfx/no_tube_connection_m.ogg");
	mVoiceTable[MALE][UNSUITABLE_LANDING_SITE] = new Sound("sfx/unsuitable_landing_site_m.ogg");

	mVoiceTable[FEMALE][CC_NO_BULLDOZE] = new Sound("sfx/cc_no_bulldoze_f.ogg");
	mVoiceTable[FEMALE][INSUFFICIENT_RESOURCES] = new Sound("sfx/insufficient_resources_f.ogg");
	mVoiceTable[FEMALE][INVALID_DIGGER_PLACEMENT] = new Sound("sfx/invalid_digger_placement_f.ogg");
	mVoiceTable[FEMALE][INVALID_MINER_PLACEMENT] = new Sound("sfx/invalid_miner_placement_f.ogg");
	mVoiceTable[FEMALE][INVALID_STRUCTURE_PLACEMENT] = new Sound("sfx/invalid_structure_placement_f.ogg");
	mVoiceTable[FEMALE][INVALID_TUBE_PLACEMENT] = new Sound("sfx/invalid_tube_placement_f.ogg");
	mVoiceTable[FEMALE][LOCATION_NOT_BULLDOZED] = new Sound("sfx/location_not_bulldozed_f.ogg");
	mVoiceTable[FEMALE][MAX_DIGGING_DEPTH_REACHED] = new Sound("sfx/max_dig_depth_f.ogg");
	mVoiceTable[FEMALE][NO_TUBE_CONNECTION] = new Sound("sfx/no_tube_connection_f.ogg");
	mVoiceTable[FEMALE][UNSUITABLE_LANDING_SITE] = new Sound("sfx/unsuitable_landing_site_f.ogg");
}