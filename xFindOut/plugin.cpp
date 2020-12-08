#include "plugin.h"
#include <TlHelp32.h>
#include <chrono>

#include "resource.h"

std::chrono::steady_clock::time_point lastEntryCreation;

enum
{
    MENU,
    DUMP_ACCESS,
    DUMP_WRITE
};

static bool addFindOutEntry(duint address, bool onWrite)
{
    lastEntryCreation = std::chrono::steady_clock::now();
    bool result = StateManager::getInstance().addEntry(address, onWrite);

    if (result)
    {
        _plugin_logprintf("Added new entry at %p\n", address);
        return true;
    }

    _plugin_logprintf("Couldn't create new entry at %p\n", address);
    return false;
}

static duint GetSelectionStart()
{
    SELECTIONDATA sel;
    GuiSelectionGet(GUI_DUMP, &sel);

    return sel.start;
}

PLUG_EXPORT void CBSTOPDEBUG(CBTYPE cbType, PLUG_CB_STOPDEBUG* info)
{
    ;
}

PLUG_EXPORT void CBPAUSEDEBUG(CBTYPE cbType, PLUG_CB_PAUSEDEBUG* info)
{
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastEntryCreation).count() < 50)
        DbgCmdExec("r");
}

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch(info->hEntry)
    {
    case DUMP_ACCESS:
    {
        auto sel = GetSelectionStart();
        if (!sel)
            return;

        addFindOutEntry(sel, false);
    }
    break;
    case DUMP_WRITE:
    {
        auto sel = GetSelectionStart();
        if (!sel)
            return;

        addFindOutEntry(sel, true);
    }
    break;
    default:
        break;
    }
}

PLUG_EXPORT void CBBREAKPOINT(CBTYPE cbType, PLUG_CB_BREAKPOINT* info)
{
    if (info->breakpoint->type != bp_hardware)
        return;

    StateManager& state = StateManager::getInstance();

    duint cip = GetContextData(UE_CIP);

    if (!state.isControlledByThisPlugin(info->breakpoint->addr))
        return;

    int index = state.wasThisInstructionHitBefore(info->breakpoint->addr, cip);
    if (index == -1)
    {
        char buffer[32];
        HitEntry hitEntry;
        std::memset(&hitEntry, 0, sizeof(hitEntry));
        hitEntry.hits = 1;

        sprintf_s(buffer, "dis.prev(%p)", cip);
        duint previousInstructionAddress = DbgEval(buffer);
        hitEntry.instructionAddress = previousInstructionAddress;
        hitEntry.hittedAtAddress = cip;

        char disassembly[128];
        GuiGetDisassembly(previousInstructionAddress, disassembly);
        snprintf(hitEntry.instruction, sizeof(hitEntry.instruction), "%p - %s", hitEntry.instructionAddress, disassembly);

        REGDUMP regdump;
        DbgGetRegDumpEx(&regdump, sizeof(regdump));

#ifdef _WIN64
        snprintf(hitEntry.info, sizeof(hitEntry.info), "RAX=%p\r\nRBX=%p\r\nRCX=%p\r\nRDX=%p\r\nRBP=%p\r\nRSP=%p\r\nRSI=%p\r\nRDI=%p\r\nRIP=%p\r\n"
            "R9=%p\r\nR10=%p\r\nR11=%p\r\nR12=%p\r\nR13=%p\r\nR14=%p\r\nR15=%p\r\n",
            regdump.regcontext.cax, regdump.regcontext.cbp, regdump.regcontext.ccx, regdump.regcontext.cdx, regdump.regcontext.cbp, regdump.regcontext.csp,
            regdump.regcontext.csi, regdump.regcontext.cdi, regdump.regcontext.cip, regdump.regcontext.r9, regdump.regcontext.r10, regdump.regcontext.r11,
            regdump.regcontext.r12, regdump.regcontext.r13, regdump.regcontext.r14, regdump.regcontext.r15);
#else
        snprintf(hitEntry.info, sizeof(hitEntry.info), "EAX=%p\r\nEBX=%p\r\nECX=%p\r\nEDX=%p\r\nEBP=%p\r\nESP=%p\r\nESI=%p\r\nEDI=%p\r\nEIP=%p\r\n",
            regdump.regcontext.cax, regdump.regcontext.cbp, regdump.regcontext.ccx, regdump.regcontext.cdx, regdump.regcontext.cbp, regdump.regcontext.csp,
            regdump.regcontext.csi, regdump.regcontext.cdi, regdump.regcontext.cip);
#endif

        state.addNewInstructionHit(info->breakpoint->addr, hitEntry);
    }

    else
        state.updateHits(info->breakpoint->addr, index);
}

static bool findOut(int argc, char* argv[])
{
    if (argc < 2)
        return false;

    duint currAddy = DbgEval(argv[1]);
    bool isWrite = false;

    if (argc == 3 && *(argv[2]) == 'w')
        isWrite = true;

    addFindOutEntry(currAddy, isWrite);

    return true;
}

static bool findOutStop(int argc, char* argv[])
{
    char command[128] = { 0 };
    duint currAddy = DbgEval(argv[1]);

    bool result = StateManager::getInstance().sendCloseMessageByAddress(currAddy);

    if (result)
        _plugin_logprintf("Entry %p has been deleted\n", currAddy);
    else
        _plugin_logprintf("Could NOT delete entry %p\n", currAddy);

    return true;
}

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    if (!_plugin_registercommand(pluginHandle, "findout", findOut, false))
        _plugin_logputs("[" PLUGIN_NAME "] Error registering the 'findout' command!");
    if (!_plugin_registercommand(pluginHandle, "findoutstop", findOutStop, false))
        _plugin_logputs("[" PLUGIN_NAME "] Error registering the 'findoutstop' command!");

    return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{
}

//Do GUI/Menu related things here.
void pluginSetup()
{
    _plugin_menuaddentry(hMenuDump, DUMP_ACCESS, "&Find out what accesses this address");
    _plugin_menuaddentry(hMenuDump, DUMP_WRITE, "&Find out what writes to this address");
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
        StateManager::getInstance().setHInstance(hinstDLL);

    return TRUE;
}

