#pragma once

#include "Common.h"
#include "Constants.h"

#include "Things/Structures/Structures.h"


class StructureFactory
{
public:

	static Structure* get(StructureID type);

protected:

private:

	// Explicitly declared private to prevent instantiation.
	StructureFactory() {}
	~StructureFactory() {}
};