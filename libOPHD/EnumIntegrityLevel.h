#pragma once


enum class IntegrityLevel
{
	Destroyed,
	Unstable,
	Decayed,
	Worn,
	Good,
	Mint,
};


IntegrityLevel integrityLevel(int integrity);
