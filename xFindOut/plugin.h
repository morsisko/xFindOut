#pragma once

#include "pluginmain.h"
#include "StateManager.h"

//plugin data
#define PLUGIN_NAME "xFindOut"
#define PLUGIN_VERSION 1

//functions
bool pluginInit(PLUG_INITSTRUCT* initStruct);
void pluginStop();
void pluginSetup();
