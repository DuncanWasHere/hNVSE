#include "hnvse.hpp"

#include "functions/hnvse_fn_region.hpp"
#include "functions/hnvse_fn_worldspace.hpp"

#include "hooks/hnvse_hk_region.hpp"

PluginHandle     g_pluginHandle = kPluginHandle_Invalid;
constexpr UInt32 g_pluginVersion = 120;
IDebugLog g_log("logs\\hNVSE.log");

bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);
NVSEInterface*              g_nvseInterface = nullptr;
NVSEDataInterface*          g_dataInterface = nullptr;
NVSESerializationInterface* g_serializationInterface = nullptr;
NVSEConsoleInterface*       g_consoleInterface = nullptr;
NVSEMessagingInterface*     g_messagingInterface = nullptr;
NVSECommandTableInterface*  g_cmdTableInterface = nullptr;
NVSEScriptInterface*        g_script = nullptr;
NVSEStringVarInterface*     g_stringInterface = nullptr;
NVSEArrayVarInterface*      g_arrayInterface = nullptr;
NVSEEventManagerInterface*  g_eventInterface = nullptr;
NVSEArrayVarInterface*      g_arrInterface = nullptr;

#define RegisterScriptCommand(name) nvse->RegisterCommand(&kCommandInfo_##name); // Default return type (return a number)
#define REG_CMD(name) nvse->RegisterCommand(&kCommandInfo_##name);                                    // from JIPLN
#define REG_TYPED_CMD(name, type) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_##type); // from JG
#define REG_CMD_STR(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_String);         // From JIPLN
#define REG_CMD_ARR(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Array);          // From JIPLN
#define REG_CMD_FORM(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Form);          // From SO
#define REG_CMD_AMB(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Ambiguous);      // From SO

static bool bMergeRegionLists = true;

void MessageHandler(NVSEMessagingInterface::Message* msg) {
	switch (msg->type) {
		case NVSEMessagingInterface::kMessage_DeferredInit:
			Console_Print("hNVSE version %.2f: Howdy neigh-bor!", (g_pluginVersion / 100.0F));
			break;
		default:
			break;
		}
}

extern "C" {

	__declspec(dllexport) bool NVSEPlugin_Query(const NVSEInterface* nvse, PluginInfo* info) {
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "hNVSE";
		info->version = g_pluginVersion;

		// Version checks
		if (nvse->nvseVersion < 0) {
			_ERROR("NVSE version too old (got %08X, expected at least %08X)", nvse->nvseVersion, PACKED_NVSE_VERSION);
			return false;
		}

		if (!nvse->isEditor) {
			if (nvse->runtimeVersion < RUNTIME_VERSION_1_4_0_525) {
				_ERROR("Incorrect runtime version (got %08X, need at least %08X)", nvse->runtimeVersion,
					   RUNTIME_VERSION_1_4_0_525);
				return false;
			}

			if (nvse->isNogore) {
				_ERROR("NoGore is not supported");
				return false;
			}
		}
		else {
			if (nvse->editorVersion < CS_VERSION_1_4_0_518) {
				_ERROR("Incorrect editor version (got %08X, need at least %08X)", nvse->editorVersion,
					   CS_VERSION_1_4_0_518);
				return false;
			}
		}

		// Version checks passed

		return true;
	}

	__declspec(dllexport) bool NVSEPlugin_Load(NVSEInterface* nvse) {
		g_pluginHandle = nvse->GetPluginHandle();
		g_nvseInterface = nvse;

		// Script Commands
		nvse->SetOpcodeBase(0x3f40);
		REG_CMD(SetWorldspaceMapBounds);
		REG_CMD_ARR(GetCellRegions);

		// Register to receive messages from NVSE
		g_messagingInterface = static_cast<NVSEMessagingInterface*>(nvse->QueryInterface(kInterface_Messaging));
		g_messagingInterface->RegisterListener(g_pluginHandle, "NVSE", MessageHandler);

		if (!nvse->isEditor) {
			// Script and function-related interfaces
			g_script = static_cast<NVSEScriptInterface*>(nvse->QueryInterface(kInterface_Script));
			g_stringInterface = static_cast<NVSEStringVarInterface*>(nvse->QueryInterface(kInterface_StringVar));
			g_arrayInterface = static_cast<NVSEArrayVarInterface*>(nvse->QueryInterface(kInterface_ArrayVar));
			g_dataInterface = static_cast<NVSEDataInterface*>(nvse->QueryInterface(kInterface_Data));
			g_eventInterface = static_cast<NVSEEventManagerInterface*>(nvse->QueryInterface(kInterface_EventManager));
			g_serializationInterface = static_cast<NVSESerializationInterface*>(nvse->QueryInterface(kInterface_Serialization));
			g_consoleInterface = static_cast<NVSEConsoleInterface*>(nvse->QueryInterface(kInterface_Console));
			g_arrInterface = static_cast<NVSEArrayVarInterface*>(nvse->QueryInterface(kInterface_ArrayVar));
			ExtractArgsEx = g_script->ExtractArgsEx;

			// INI Loading (from Wall (from karut))
			_MESSAGE("Reading INI...");
			char cINIDir[MAX_PATH];
			GetModuleFileNameA(GetModuleHandle(NULL), cINIDir, MAX_PATH);
			strcpy((char*)(strrchr(cINIDir, '\\') + 1), "Data\\NVSE\\Plugins\\hNVSE.ini");
			bMergeRegionLists = GetPrivateProfileInt("Main", "bMergeRegionLists", 0, cINIDir);
			_MESSAGE("Done.");

			// Hooks
			if (bMergeRegionLists) {
				WriteRelJump(0x4151FD, (UInt32)HookRegionListLoad); // Merge cell region lists between mods during loading
				_MESSAGE("Applied region list load hook.");
			}
		}

		return true;
	}
};
