#pragma once

#include "nvse/GameAPI.h"
#include "nvse/PluginAPI.h"
#include "nvse/ParamInfos.h"
#include "nvse/SafeWrite.h"
#include "nvse/GameTypes.h"

extern bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);
extern NVSEInterface*              g_nvseInterface;
extern NVSEDataInterface*          g_dataInterface;
extern NVSEMessagingInterface*     g_messagingInterface;
extern NVSECommandTableInterface*  g_cmdTableInterface;
extern NVSEScriptInterface*        g_script;
extern NVSEStringVarInterface*     g_stringInterface;
extern NVSESerializationInterface* g_serializationInterface;
extern NVSEConsoleInterface*       g_consoleInterface;
extern NVSEEventManagerInterface*  g_eventInterface;
extern NVSEArrayVarInterface*      g_arrInterface;

// From Wall
#define LOGGING 0

#if LOGGING
#define DEBUG_MSG(...) _MESSAGE(__VA_ARGS__)
#else
#define DEBUG_MSG(...)
#endif
