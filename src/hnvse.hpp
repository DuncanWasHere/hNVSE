#pragma once

#include "nvse/GameAPI.h"
#include "nvse/PluginAPI.h"
#include "nvse/ParamInfos.h"

// NVSE Globals
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

static ParamInfo kParams_OneWorldSpace_FourInts[5] = {
    {"world space", kParamType_WorldSpace, 0},
    {"nw x", kParamType_Integer, 0},
    {"nw y", kParamType_Integer, 0},
    {"se x", kParamType_Integer, 0},
    {"se y", kParamType_Integer, 0},
};
