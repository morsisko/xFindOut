#include "plugin.h"
#include <TlHelp32.h>

enum
{
    MENU,
    DUMP
};

PLUG_EXPORT void CBINITDEBUG(CBTYPE cbType, PLUG_CB_INITDEBUG* info)
{
    dprintf("Debugging of %s started!\n", info->szFileName);
}

PLUG_EXPORT void CBSTOPDEBUG(CBTYPE cbType, PLUG_CB_STOPDEBUG* info)
{
    dputs("Debugging stopped!");
}

PLUG_EXPORT void CBEXCEPTION(CBTYPE cbType, PLUG_CB_EXCEPTION* info)
{
    dprintf("ExceptionRecord.ExceptionCode: %08X\n", info->Exception->ExceptionRecord.ExceptionCode);
}

PLUG_EXPORT void CBDEBUGEVENT(CBTYPE cbType, PLUG_CB_DEBUGEVENT* info)
{
    if(info->DebugEvent->dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
    {
        dprintf("DebugEvent->EXCEPTION_DEBUG_EVENT->%.8X\n", info->DebugEvent->u.Exception.ExceptionRecord.ExceptionCode);
    }
}

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch(info->hEntry)
    {
    case DUMP:
    default:
        break;
    }
}

int hits = 0;
duint currAddy = 0;
PLUG_EXPORT void CBBREAKPOINT(CBTYPE cbType, PLUG_CB_BREAKPOINT* info)
{
    dprintf("Breakpoint %d\n", info->breakpoint->addr);
    
    if (info->breakpoint->addr == currAddy)
        hits++;
}
static bool findOut(int argc, char* argv[])
{
    if (argc < 2)
        return false;

    currAddy = DbgEval(argv[1]);
    hits = 0;

    char command[128] = "";
    sprintf_s(command, "bph %p", currAddy);
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
    sprintf_s(command, "bphc %p", currAddy);
    DbgCmdExecDirect(command);
    _plugin_logprintf("This breakpoint was hit %d times\n", hits);
    currAddy = 0;
    hits = 0;

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
