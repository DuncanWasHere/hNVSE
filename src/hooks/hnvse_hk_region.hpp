#pragma once

#include <unordered_map>

#include "nvse/GameData.h"

#include "internal/hnvse_utility.hpp"

constexpr UInt32 kAddr_HookRegionListLoad_Return = 0x415202;

constexpr UInt32 kAddr_RegionList_UpdateRegionList = 0x41BBD0;

std::unordered_map<UInt32, ExtraCellRegionList*> g_mergedRegionLists;

void __stdcall hkRegionListLoad(ExtraDataList* thisExtraDataList, ModInfo* apFile, TESObjectCELL* apCell) {
    if (!thisExtraDataList || !apFile || !apCell)
        return;

    auto xCellRegionList = (ExtraCellRegionList*)thisExtraDataList->GetByType(kExtraData_RegionList);
    if (!xCellRegionList || !xCellRegionList->regionList)
        return;

    UInt32 cellID = apCell->refID;
    TESRegionList* loadedRegionList = xCellRegionList->regionList;

    DEBUG_MSG("======================");
    DEBUG_MSG("Loading regions for cell %08X (%s) from file %s:",
        cellID, apCell->GetEditorID(), apFile->name);

    for (auto iter = loadedRegionList->list.Head(); iter; iter = iter->next) {
        TESRegion* region = iter->data;
        if (region) {
            DEBUG_MSG("  - %s (%08X)", region->GetEditorID(), region->refID);
        }
    }

    // Find or create merged ExtraCellRegionList for this cell
    ExtraCellRegionList* mergedExtra = g_mergedRegionLists[cellID];
    if (!mergedExtra) {
        DEBUG_MSG("First time this cell has been loaded! Initializing merged region list.");
        mergedExtra = ExtraCellRegionList::Create();
        g_mergedRegionLists[cellID] = mergedExtra;
    }

    // Append new regions to merged list
    for (auto iter = loadedRegionList->list.Head(); iter; iter = iter->next) {
        TESRegion* region = iter->data;

        if (region && !(mergedExtra->regionList->list.IsInList(region))) {
            mergedExtra->regionList->list.Append(region);
            DEBUG_MSG("  - Added region to merged list: %s (%08X)", region->GetEditorID(), region->refID);
        }
    }

    // Overwrite the actual region list with the merged one
    DEBUG_MSG("Merged region list for cell %08X:", cellID);
    for (auto iter = mergedExtra->regionList->list.Head(); iter; iter = iter->next) {
        TESRegion* region = iter->data;
        
        if (region) {        
            if (!(xCellRegionList->regionList->list.IsInList(region)))
                xCellRegionList->regionList->list.Append(region);

            DEBUG_MSG("  - %s (%08X)", region->GetEditorID(), region->refID);
        }
    }
}

void __declspec(naked) HookRegionListLoad() {
	__asm {
		// Vanilla instruction
		call kAddr_RegionList_UpdateRegionList

		// Save vanilla registers
		pushad
		pushfd

		// Call hook function
		mov eax, [ebp + 0x0C]  // TESObjectCELL*
		push eax
		mov eax, [ebp + 0x08]  // TESFile* (ModInfo in NVSE)
		push eax
		mov eax, [ebp - 0x358] // ExtraDataList*
		push eax

		call hkRegionListLoad

		// Restore vanilla registers
		popfd
		popad

		// Return
		jmp kAddr_HookRegionListLoad_Return
	}
}
