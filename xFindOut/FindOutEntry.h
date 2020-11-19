#pragma once
#include "HitEntry.h"
#include <vector>
#include <memory>
#include <algorithm>

class FindOutEntry
{
private:
	duint breakpointAddress;
	std::vector<std::unique_ptr<HitEntry>> hits;
};
