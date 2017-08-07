#include "GameStateHelper.h"

#include "../Constants.h"

using namespace NAS2D::Xml;

bool checkTubeConnection(Tile* _tile, Direction _dir, ConnectorDir _source_connector_dir)
{
	if (_tile->mine() || !_tile->bulldozed() || !_tile->excavated() || !_tile->thingIsStructure())
		return false;

	Structure* structure = _tile->structure();

	if (_source_connector_dir == CONNECTOR_INTERSECTION)
	{
		if (_dir == DIR_EAST || _dir == DIR_WEST)
		{
			if (structure->connectorDirection() == CONNECTOR_INTERSECTION || structure->connectorDirection() == CONNECTOR_RIGHT || structure->connectorDirection() == CONNECTOR_VERTICAL)
				return true;
		}
		else // NORTH/SOUTH
		{
			if (structure->connectorDirection() == CONNECTOR_INTERSECTION || structure->connectorDirection() == CONNECTOR_LEFT || structure->connectorDirection() == CONNECTOR_VERTICAL)
				return true;
		}
	}
	else if (_source_connector_dir == CONNECTOR_RIGHT && (_dir == DIR_EAST || _dir == DIR_WEST))
	{
		if (structure->connectorDirection() == CONNECTOR_INTERSECTION || structure->connectorDirection() == CONNECTOR_RIGHT || structure->connectorDirection() == CONNECTOR_VERTICAL)
			return true;
	}
	else if (_source_connector_dir == CONNECTOR_LEFT && (_dir == DIR_NORTH || _dir == DIR_SOUTH))
	{
		if (structure->connectorDirection() == CONNECTOR_INTERSECTION || structure->connectorDirection() == CONNECTOR_LEFT || structure->connectorDirection() == CONNECTOR_VERTICAL)
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
	{
		if (_sl[i]->operational())
		{
			storage += _sl[i]->storage().capacity();
		}
	}

	return constants::BASE_STORAGE_CAPACITY + storage;
}


void updateRobotControl(RobotPool& _rp, StructureManager& _sm)
{
	auto CommandCenter = _sm.structureList(Structure::CLASS_COMMAND);
	auto RobotCommand = _sm.structureList(Structure::CLASS_ROBOT_COMMAND);

	// 3 for the first command center
	uint32_t _maxRobots = 0;
	if (CommandCenter.size() > 0) { _maxRobots += 3; }
	// the 10 per robot command facility
	for (size_t s = 0; s < RobotCommand.size(); ++s)
	{
		if (RobotCommand[s]->operational()) { _maxRobots += 10; }
	}

	_rp.InitRobotCtrl(_maxRobots);
}


bool structureIsLander(StructureID id)
{
	return id == SID_SEED_LANDER || id == SID_COLONIST_LANDER;
}


bool selfSustained(StructureID id)
{
	return id == SID_COMM_TOWER;
}


bool outOfCommRange(StructureManager& sm, Point_2d& cc_location, TileMap* tile_map, Tile* current_tile)
{
	Tile* tile = tile_map->getVisibleTile();

	if (tile->distanceTo(tile_map->getTile(cc_location.x(), cc_location.y(), 0)) <= constants::ROBOT_COM_RANGE)
		return false;

	Tile* _comm_t = nullptr;
	for (auto _tower : sm.structureList(Structure::CLASS_COMM))
	{
		if (!_tower->operational())
		{
			continue;
		}

		_comm_t = sm.tileFromStructure(_tower);
		if (_comm_t->distanceTo(current_tile) <= constants::COMM_TOWER_BASE_RANGE)
		{
			return false;
		}
	}

	return true;
}


// ==============================================================
// = CONVENIENCE FUNCTIONS FOR WRITING OUT GAME STATE INFORMATION
// ==============================================================
void checkRobotDeployment(XmlElement* _ti, RobotTileTable& _rm, Robot* _r, RobotType _type)
{
	_ti->attribute("type", _type);
	_ti->attribute("age", _r->fuelCellAge());
	_ti->attribute("production", _r->turnsToCompleteTask());

	for (auto it = _rm.begin(); it != _rm.end(); ++it)
	{
		if (it->first == _r)
		{
			_ti->attribute("x", it->second->x());
			_ti->attribute("y", it->second->y());
			_ti->attribute("depth", it->second->depth());
		}
	}

}


// Convenience function
void writeRobots(XmlElement* _ti, RobotPool& _rp, RobotTileTable& _rm)
{
	XmlElement* robots = new XmlElement("robots");

	RobotPool::DiggerList& diggers = _rp.diggers();
	for (size_t i = 0; i < diggers.size(); ++i)
	{
		XmlElement* robot = new XmlElement("robot");
		checkRobotDeployment(robot, _rm, static_cast<Robot*>(diggers[i]), ROBOT_DIGGER);
		robot->attribute("direction", diggers[i]->direction());
		robots->linkEndChild(robot);
	}

	RobotPool::DozerList& dozers = _rp.dozers();
	for (size_t i = 0; i < dozers.size(); ++i)
	{
		XmlElement* robot = new XmlElement("robot");
		checkRobotDeployment(robot, _rm, static_cast<Robot*>(dozers[i]), ROBOT_DOZER);
		robots->linkEndChild(robot);
	}

	RobotPool::MinerList& miners = _rp.miners();
	for (size_t i = 0; i < miners.size(); ++i)
	{
		XmlElement* robot = new XmlElement("robot");
		checkRobotDeployment(robot, _rm, static_cast<Robot*>(miners[i]), ROBOT_MINER);
		robots->linkEndChild(robot);
	}

	_ti->linkEndChild(robots);
}


void writeResources(XmlElement* _ti, ResourcePool& _rp)
{
	XmlElement* resources = new XmlElement("resources");
	_rp.serialize(resources);
	_ti->linkEndChild(resources);
}


void readResources(XmlElement* _ti, ResourcePool& _rp)
{
	if (_ti)
		_rp.deserialize(_ti);
}
