#pragma once

#include "internal/hnvse_utility.hpp"

DEFINE_COMMAND_PLUGIN(GetCellRegions, "Returns an exterior cell's region list.", false, kParams_OneForm);

bool Cmd_GetCellRegions_Execute(COMMAND_ARGS) {
	TESObjectCELL* apCell = nullptr;
	NVSEArrayVarInterface::Array* outArr = g_arrInterface->CreateArray(nullptr, 0, scriptObj);

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &apCell)) {
		auto xRegionList = static_cast<ExtraCellRegionList*>(apCell->extraDataList.GetByType(kExtraData_RegionList));

		for (auto i = xRegionList->regionList->list.Head(); i; i = i->next) {
			TESRegion* region = i->data;
			if (region) {
				g_arrInterface->AppendElement(outArr, NVSEArrayVarInterface::ElementL(region));
				if (IsConsoleMode())
					Console_Print("  >> %08X (%s)", region->refID, region->GetEditorID());
			}
		}
	}

	g_arrInterface->AssignCommandResult(outArr, result);
	return true;
}
