#pragma once

#include "hnvse.hpp"

DEFINE_COMMAND_PLUGIN(SetWorldspaceMapBounds, "Sets the NW and SE cell coordinates for a worldspace's map.", false, kParams_OneWorldSpace_FourInts);

bool Cmd_SetWorldspaceMapBounds_Execute(COMMAND_ARGS) {
	TESWorldSpace* aWorldSpace = nullptr;
	signed short aNW_X = 0;
	signed short aNW_Y = 0;
	signed short aSE_X = 0;
	signed short aSE_Y = 0;
	*result = true;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &aWorldSpace, &aNW_X, &aNW_Y, &aSE_X, &aSE_Y)) {
		aWorldSpace->mapData.cellNWCoordinates = { aNW_X, aNW_Y };
		aWorldSpace->mapData.cellSECoordinates = { aSE_X, aSE_Y };
	}

	return true;
}
