#include "plugin.h"
#include <TlHelp32.h>

#include "resource.h"

enum
{
    MENU,
    DUMP
};

PLUG_EXPORT void CBSTOPDEBUG(CBTYPE cbType, PLUG_CB_STOPDEBUG* info)
{
    dputs("Debugging stopped!");
}

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch(info->hEntry)
    {
    case DUMP:
    {

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
        sprintf_s(buffer, "dis.prev(%p)", cip);
        duint previousInstructionAddress = DbgEval(buffer);
        hitEntry.instructionAddress = previousInstructionAddress;
        hitEntry.hittedAtAddress = cip;

        char disassembly[128];
        GuiGetDisassembly(previousInstructionAddress, disassembly);
        snprintf(hitEntry.instruction, sizeof(hitEntry.instruction), "%X - %s", hitEntry.instructionAddress, disassembly);

        REGDUMP regdump;
        DbgGetRegDumpEx(&regdump, sizeof(regdump));

        snprintf(hitEntry.info, sizeof(hitEntry.info), "EAX=%p\nEBX=%p\nECX=%p\nEDX=%p\nEBP=%p\nESP=%p\nESI=%p\nEDI=%p\nEIP=%p\n",
            regdump.regcontext.cax, regdump.regcontext.cbp, regdump.regcontext.ccx, regdump.regcontext.cdx, regdump.regcontext.cbp, regdump.regcontext.csp,
            regdump.regcontext.csi, regdump.regcontext.cdi, regdump.regcontext.cip);

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

    StateManager::getInstance().addEntry(currAddy);

    char command[128] = "";
    sprintf_s(command, "bph %p, r", currAddy);
    DbgCmdExecDirect(command);

    sprintf_s(command, "bphwcond %p, 0", currAddy);
    _plugin_logputs(command);
    _plugin_logputs("\n");
    DbgCmdExecDirect(command);

    return true;
}

static bool findOutStop(int argc, char* argv[])
{
    char command[128] = "";
    duint currAddy = DbgEval(argv[1]);
    sprintf_s(command, "bphc %p", currAddy);
    DbgCmdExecDirect(command);
    //for (const auto& value : hitsMap)
    //    _plugin_logprintf("Address %p, %s, hits: %d\n%s\n\n\n", value.second.previousInstruction, value.second.disassembly, value.second.hits, value.second.regDump);
    
    currAddy = 0;

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
    _plugin_menuaddentry(hMenu, DUMP, "&xFindOut");
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
        StateManager::getInstance().setHInstance(hinstDLL);

    return TRUE;
}

