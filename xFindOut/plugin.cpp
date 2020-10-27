#include "plugin.h"

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

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
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
