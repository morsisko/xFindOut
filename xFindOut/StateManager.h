#pragma once
#include "FindOutEntry.h"
#include <memory>
#include <vector>
#include <algorithm>

class StateManager
{
private:
	std::vector<std::unique_ptr<FindOutEntry>> entries;
	auto getEntryIteratorByBreakpointAddress(duint breakpointAddress);
	HINSTANCE instance;
public:
	static StateManager& getInstance();
	bool isControlledByThisPlugin(duint breakpointAddress);
	int wasThisInstructionHitBefore(duint breakpointAddress, duint instructionAddress);
	void updateHits(duint breakpointAddress, int index);
	void addNewInstructionHit(duint breakpointAddress, HitEntry& hitEntry);
	bool addEntry(duint breakpointAddress);
	void setHInstance(HINSTANCE instance);
	HINSTANCE getHInstance();
	void debugLog();
	char* getInfoByHwndAndIndex(HWND hwnd, int index);
	duint getInstructionAddressByHwndAndIndex(HWND hwnd, int index);
	bool sendCloseMessageByAddress(duint breakpointAddress);
	bool deleteEntry(HWND hwnd);
};

