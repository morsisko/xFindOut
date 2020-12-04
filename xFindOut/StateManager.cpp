#include "StateManager.h"

auto StateManager::getEntryIteratorByBreakpointAddress(duint breakpointAddress)
{
    return std::find_if(entries.begin(), entries.end(), [breakpointAddress](const auto& entry) {
        return entry->getBreakpointAddress() == breakpointAddress;
    });
}

void StateManager::setHInstance(HINSTANCE instance)
{
    this->instance = instance;
}

StateManager& StateManager::getInstance()
{
    static StateManager manager;
    return manager;
}

bool StateManager::isControlledByThisPlugin(duint breakpointAddress)
{
    return getEntryIteratorByBreakpointAddress(breakpointAddress) != entries.end();
}

int StateManager::wasThisInstructionHitBefore(duint breakpointAddress, duint instructionAddress)
{
    auto it = getEntryIteratorByBreakpointAddress(breakpointAddress);

    if (it == entries.end())
        return -1;

    return it->get()->wasHitBefore(instructionAddress);
}

void StateManager::updateHits(duint breakpointAddress, int index)
{
    auto it = getEntryIteratorByBreakpointAddress(breakpointAddress);

    if (it == entries.end())
        return;

    return it->get()->hit(index);
}

void StateManager::addNewInstructionHit(duint breakpointAddress, HitEntry& hitEntry)
{
    auto it = getEntryIteratorByBreakpointAddress(breakpointAddress);

    if (it == entries.end())
        return;

    it->get()->addNew(hitEntry);
}

bool StateManager::addEntry(duint breakpointAddress)
{
    if (getEntryIteratorByBreakpointAddress(breakpointAddress) != entries.end())
        return false;

    char command[128] = "";
    sprintf_s(command, "bph %p, r", breakpointAddress);
    DbgCmdExecDirect(command);

    sprintf_s(command, "bphwcond %p, 0", breakpointAddress);
    DbgCmdExecDirect(command);

    entries.push_back(std::make_unique<FindOutEntry>(breakpointAddress));
    return true;
}

HINSTANCE StateManager::getHInstance()
{
    return instance;
}

void StateManager::debugLog()
{
    for (const auto& entry : entries)
        entry->debugLog();
}

char* StateManager::getInfoByHwndAndIndex(HWND hwnd, int index)
{
    auto it = std::find_if(entries.begin(), entries.end(), [hwnd](const auto& entry) {
        return entry->getDialog() == hwnd;
    });

    if (it == entries.end())
        return nullptr;

    return it->get()->getInfoByIndex(index);
}

duint StateManager::getInstructionAddressByHwndAndIndex(HWND hwnd, int index)
{
    auto it = std::find_if(entries.begin(), entries.end(), [hwnd](const auto& entry) {
        return entry->getDialog() == hwnd;
        });

    if (it == entries.end())
        return 0;

    return it->get()->getInstructionAddressByIndex(index);
}

bool StateManager::sendCloseMessageByAddress(duint breakpointAddress)
{
    auto it = getEntryIteratorByBreakpointAddress(breakpointAddress);

    if (it == entries.end())
        return false;

    SendMessage(it->get()->getDialog(), WM_CLOSE, 0, 0);
    return true;
}

bool StateManager::deleteEntry(HWND hwnd)
{
    auto it = std::find_if(entries.begin(), entries.end(), [hwnd](const auto& entry) {
        return entry->getDialog() == hwnd;
        });

    if (it == entries.end())
        return false;

    //tricky solution to bypass x64dbg bug
    char command[128];
    sprintf_s(command, "bphwcond %p, 1 %p", it->get()->getBreakpointAddress());
    DbgCmdExecDirect(command);
    Sleep(10);
    sprintf_s(command, "bphc %p", it->get()->getBreakpointAddress());
    DbgCmdExecDirect(command);
    Sleep(10);
    DbgCmdExec("r");

    entries.erase(it);
    return true;
}
