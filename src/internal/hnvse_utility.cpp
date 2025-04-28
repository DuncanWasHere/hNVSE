#include "hnvse_utility.hpp"

ExtraCellRegionList* ExtraCellRegionList::Create() {
	auto data = (ExtraCellRegionList*)GameHeapAlloc(sizeof(ExtraCellRegionList));
	if (data) {
		memset(data, 0, sizeof(ExtraCellRegionList));
		data->type = kExtraData_RegionList;

		auto dataRegionList = (TESRegionList*)GameHeapAlloc(sizeof(TESRegionList));
		if (dataRegionList) {
			memset(dataRegionList, 0, sizeof(TESRegionList));
			data->regionList = dataRegionList;
		}
	}

	return data;
}
