#pragma once
#include "pluginsdk/bridgemain.h"

struct HitEntry
{
	duint hittedAtAddress;
	duint instructionAddress;
	int hits;
	char instruction[64];
	char info[512];
};