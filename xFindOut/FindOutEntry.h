#pragma once
#include "HitEntry.h"
#include "HitDialog.h"
#include <vector>
#include <memory>
#include <algorithm>

class FindOutEntry
{
private:
	duint breakpointAddress;
	std::vector<std::unique_ptr<HitEntry>> hits;
	HitDialog dialog;
	bool enabled;
public:
	FindOutEntry(duint breakpointAddress, bool isWriteEntry);
	duint getBreakpointAddress();
	int wasHitBefore(duint cip);
	void hit(int index);
	void addNew(HitEntry& hitEntry);
	HWND getDialog();
	void debugLog();
	char* getInfoByIndex(int index);
	duint getInstructionAddressByIndex(int index);
	bool isEnabled();
	void disable();
};
