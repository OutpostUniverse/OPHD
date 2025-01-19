#include "Resources.h"


const std::array<std::string, 4> ResourceNamesRefined =
{
	"Common Metals",
	"Common Minerals",
	"Rare Metals",
	"Rare Minerals",
};


const std::array<std::string, 4> ResourceNamesOre =
{
	"Common Metals Ore",
	"Common Minerals Ore",
	"Rare Metals Ore",
	"Rare Minerals Ore",
};


const std::array<NAS2D::Rectangle<int>, 4> ResourceImageRectsRefined =
{
	NAS2D::Rectangle<int>{{64, 16}, {16, 16}},
	NAS2D::Rectangle<int>{{96, 16}, {16, 16}},
	NAS2D::Rectangle<int>{{80, 16}, {16, 16}},
	NAS2D::Rectangle<int>{{112, 16}, {16, 16}},
};


const std::array<NAS2D::Rectangle<int>, 4> ResourceImageRectsOre =
{
	NAS2D::Rectangle<int>{{64, 0}, {16, 16}},
	NAS2D::Rectangle<int>{{96, 0}, {16, 16}},
	NAS2D::Rectangle<int>{{80, 0}, {16, 16}},
	NAS2D::Rectangle<int>{{112, 0}, {16, 16}},
};
