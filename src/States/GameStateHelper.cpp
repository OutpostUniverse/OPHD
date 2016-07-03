#include "GameStateHelper.h"

#include "../Constants.h"

// Throw away string stream for font rendering.
stringstream str;

void drawString(Renderer& r, Font& f, std::string s, int i, int x, int y, int red, int green, int blue)
{
	str.str("");
	str << s << i;
	r.drawText(f, str.str(), (float)x, (float)y, red, green, blue);
}


void drawNumber(Renderer& r, Font& f, int i, int x, int y, int red, int green, int blue)
{
	str.str("");
	str << i;
	r.drawText(f, str.str(), (float)x, (float)y, red, green, blue);
}


bool checkTubeConnection(Tile* tile, Direction dir, StructureID type)
{
	if (tile->mine() || !tile->bulldozed() || !tile->excavated() || !tile->thingIsStructure())
		return false;

	Structure* _structure = tile->structure();

	if (type == SID_TUBE_INTERSECTION)
	{
		if (dir == DIR_EAST || dir == DIR_WEST)
		{
			if (_structure->connectorDirection() == CONNECTOR_INTERSECTION || _structure->connectorDirection() == CONNECTOR_RIGHT || _structure->connectorDirection() == CONNECTOR_VERTICAL)
				return true;
		}
		else // NORTH/SOUTH
		{
			if (_structure->connectorDirection() == CONNECTOR_INTERSECTION || _structure->connectorDirection() == CONNECTOR_LEFT || _structure->connectorDirection() == CONNECTOR_VERTICAL)
				return true;
		}
	}
	else if (type == SID_TUBE_RIGHT && (dir == DIR_EAST || dir == DIR_WEST))
	{
		if (_structure->connectorDirection() == CONNECTOR_INTERSECTION || _structure->connectorDirection() == CONNECTOR_RIGHT || _structure->connectorDirection() == CONNECTOR_VERTICAL)
			return true;
	}
	else if (type == SID_TUBE_LEFT && (dir == DIR_NORTH || dir == DIR_SOUTH))
	{
		if (_structure->connectorDirection() == CONNECTOR_INTERSECTION || _structure->connectorDirection() == CONNECTOR_LEFT || _structure->connectorDirection() == CONNECTOR_VERTICAL)
			return true;
	}

	return false;
}


bool checkStructurePlacement(Tile *tile, Direction dir)
{
	if (tile->mine() || !tile->bulldozed() || !tile->excavated() || !tile->thingIsStructure() || !tile->connected())
		return false;

	Structure* _structure = tile->structure();
	if (!_structure->isConnector())
		return false;

	if (dir == DIR_EAST || dir == DIR_WEST)
	{
		if (_structure->connectorDirection() == CONNECTOR_INTERSECTION || _structure->connectorDirection() == CONNECTOR_RIGHT)
			return true;
	}
	else // NORTH/SOUTH
	{
		if (_structure->connectorDirection() == CONNECTOR_INTERSECTION || _structure->connectorDirection() == CONNECTOR_LEFT)
			return true;
	}

	return false;
}


int totalStorage(StructureManager::StructureList& _sl)
{
	int storage = 0;
	for (size_t i = 0; i < _sl.size(); ++i)
		if (_sl[i]->operational())
			storage += _sl[i]->storage().capacity();

	return constants::BASE_STORAGE_CAPACITY + storage;
}


bool insertRobotIntoTable(RobotTileTable& _rm, Robot* _r, Tile* _t)
{
	if (!_t)
		return false;

	auto it = _rm.find(_r);
	if (it != _rm.end())
		throw Exception(0, "Duplicate Robot", "GameState::insertRobot(): Attempting to add a duplicate Robot* pointer.");

	_rm[_r] = _t;
	_t->pushThing(_r);

	return true;
}


// ==============================================================
// = CONVENIENCE FUNCTIONS FOR WRITING OUT GAME STATE INFORMATION
// ==============================================================
void checkRobotDeployment(TiXmlElement* _ti, RobotTileTable& _rm, Robot* _r, RobotType _type)
{
	_ti->SetAttribute("type", _type);
	_ti->SetAttribute("age", _r->fuelCellAge());
	_ti->SetAttribute("production", _r->turnsToCompleteTask());
	_ti->SetAttribute("deployed", false);

	for (auto it = _rm.begin(); it != _rm.end(); ++it)
	{
		if (it->first == _r)
		{
			_ti->SetAttribute("deployed", true);
			_ti->SetAttribute("x", it->second->x());
			_ti->SetAttribute("y", it->second->y());
			_ti->SetAttribute("depth", it->second->depth());
		}
	}

}

// Convenience function
void writeRobots(TiXmlElement* _ti, RobotPool& _rp, RobotTileTable& _rm)
{
	TiXmlElement* robots = new TiXmlElement("robots");

	RobotPool::DiggerList& diggers = _rp.diggers();
	for (size_t i = 0; i < diggers.size(); ++i)
	{
		TiXmlElement* robot = new TiXmlElement("robot");
		checkRobotDeployment(robot, _rm, static_cast<Robot*>(diggers[i]), ROBOT_DIGGER);
		robot->SetAttribute("direction", diggers[i]->direction());
		robots->LinkEndChild(robot);
	}

	RobotPool::DozerList& dozers = _rp.dozers();
	for (size_t i = 0; i < dozers.size(); ++i)
	{
		TiXmlElement* robot = new TiXmlElement("robot");
		checkRobotDeployment(robot, _rm, static_cast<Robot*>(dozers[i]), ROBOT_DOZER);
		robots->LinkEndChild(robot);
	}

	RobotPool::MinerList& miners = _rp.miners();
	for (size_t i = 0; i < miners.size(); ++i)
	{
		TiXmlElement* robot = new TiXmlElement("robot");
		checkRobotDeployment(robot, _rm, static_cast<Robot*>(miners[i]), ROBOT_MINER);
		robots->LinkEndChild(robot);
	}

	_ti->LinkEndChild(robots);
}


void writeResources(TiXmlElement* _ti, ResourcePool& _rp)
{
	TiXmlElement* resources = new TiXmlElement("resources");
	_rp.serialize(resources);
	_ti->LinkEndChild(resources);
}
