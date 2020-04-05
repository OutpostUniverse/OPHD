#pragma once

#include "Structure.h"

#include "../../Constants.h"

class SurfacePolice : public Structure
{
public:
	SurfacePolice() : Structure(constants::SURFACE_POLICE, "structures/police_surface.sprite", CLASS_SURFACE_POLICE)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
	}

protected:

	virtual void think()
	{
	}

	virtual void defineResourceInput()
	{
		resourcesIn().energy(5);
	}

	virtual void defineResourceOutput()
	{}

private:

};

class UndergroundPolice : public Structure
{
public:
	UndergroundPolice() : Structure(constants::UNDERGROUND_POLICE, "structures/police_underground.sprite", CLASS_UNDERGROUND_POLICE)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(true);
	}

protected:

	virtual void think()
	{
	}

	virtual void defineResourceInput()
	{
		resourcesIn().energy(1);
	}

	virtual void defineResourceOutput()
	{}

private:

};
