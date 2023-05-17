#include "Robominer.h"

#include "../../Constants/Strings.h"


Robominer::Robominer() :
	Robot(constants::Robominer, "robots/robominer.sprite", Robot::Type::Miner)
{
}
