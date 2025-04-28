#pragma once

#include "internal/hnvse_utility.hpp"

DEFINE_COMMAND_PLUGIN(SetWorldspaceMapBounds, "Sets the northwest and southeast cell coordinates of a worldspace's map.", false, kParams_OneWorldSpace_FourInts);

bool Cmd_SetWorldspaceMapBounds_Execute(COMMAND_ARGS) {
	*result = false;
	TESWorldSpace* aWorldSpace = nullptr;
	signed short aNW_X = 0;
	signed short aNW_Y = 0;
	signed short aSE_X = 0;
	signed short aSE_Y = 0;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &aWorldSpace, &aNW_X, &aNW_Y, &aSE_X, &aSE_Y)) {
		aWorldSpace->mapData.cellNWCoordinates = { aNW_X, aNW_Y };
		aWorldSpace->mapData.cellSECoordinates = { aSE_X, aSE_Y };
	}

	return true;
}
