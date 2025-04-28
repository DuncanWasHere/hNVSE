#pragma once

#include <atomic>

#include "nvse/PluginAPI.h"
#include "nvse/ParamInfos.h"
#include "nvse/GameExtraData.h"
#include "../hnvse.hpp"

// ExtraDataList::AddExtra (from JIP)
constexpr UInt32 kAddr_ExtraDataList_Add = 0x0040E640;
using AddExtraFunc = BSExtraData * (__fastcall*)(ExtraDataList* list, void*, BSExtraData* toAdd);
static AddExtraFunc AddExtraData = (AddExtraFunc)kAddr_ExtraDataList_Add;

static ParamInfo kParams_OneWorldSpace_FourInts[5] = {
    {"worldspace", kParamType_WorldSpace, 0},
    {"xCoordNW", kParamType_Integer, 0},
    {"yCoordNW", kParamType_Integer, 0},
    {"xCoordSE", kParamType_Integer, 0},
    {"yCoordSE", kParamType_Integer, 0},
};

class ExtraCellRegionList : public BSExtraData {
public:
    TESRegionList* regionList;

    static ExtraCellRegionList* Create();
};
