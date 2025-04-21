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
    {"worldspace", kParamType_WorldSpace, 0},
    {"xCoordNW", kParamType_Integer, 0},
    {"yCoordNW", kParamType_Integer, 0},
    {"xCoordSE", kParamType_Integer, 0},
    {"yCoordSE", kParamType_Integer, 0},
};
