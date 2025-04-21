#include "hnvse.hpp"

// Script Functions
#include "functions/hnvse_fn_worldspace.hpp"

// Plugin Globals
IDebugLog        g_log;
PluginHandle     g_pluginHandle = kPluginHandle_Invalid;
constexpr UInt32 g_pluginVersion = 110;

// NVSE Globals
bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);
NVSEInterface*             g_nvseInterface = nullptr;
NVSEDataInterface*         g_dataInterface = nullptr;
NVSESerializationInterface* g_serializationInterface = nullptr;
NVSEConsoleInterface*      g_consoleInterface = nullptr;
NVSEMessagingInterface*    g_messagingInterface = nullptr;
NVSECommandTableInterface* g_cmdTableInterface = nullptr;
NVSEScriptInterface*       g_script = nullptr;
NVSEStringVarInterface*    g_stringInterface = nullptr;
NVSEArrayVarInterface*     g_arrayInterface = nullptr;
NVSEEventManagerInterface* g_eventInterface = nullptr;

// Macros
#define RegisterScriptCommand(name) nvse->RegisterCommand(&kCommandInfo_##name); // Default return type (return a number)
#define REG_CMD(name) nvse->RegisterCommand(&kCommandInfo_##name);                                    // from JIPLN
#define REG_TYPED_CMD(name, type) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_##type); // from JG
#define REG_CMD_STR(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_String);         // From JIPLN
#define REG_CMD_ARR(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Array);          // From JIPLN
#define REG_CMD_FORM(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Form);          // From SO
#define REG_CMD_AMB(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Ambiguous);      // From SO

// This is a message handler for NVSE events
// With this, plugins can listen to messages such as whenever the game loads
void MessageHandler(NVSEMessagingInterface::Message* msg) {
    switch (msg->type) {

        /* Unused for now
        case NVSEMessagingInterface::kMessage_PostLoad: break;
        case NVSEMessagingInterface::kMessage_ExitGame: break;
        case NVSEMessagingInterface::kMessage_ExitToMainMenu: break;
        case NVSEMessagingInterface::kMessage_LoadGame: break;
        case NVSEMessagingInterface::kMessage_SaveGame: break;
    #if EDITOR
        case NVSEMessagingInterface::kMessage_ScriptEditorPrecompile: break;
    #endif
        case NVSEMessagingInterface::kMessage_PreLoadGame: break;
        case NVSEMessagingInterface::kMessage_ExitGame_Console: break;
        case NVSEMessagingInterface::kMessage_PostLoadGame: break;
        case NVSEMessagingInterface::kMessage_PostPostLoad: break;
        case NVSEMessagingInterface::kMessage_RuntimeScriptError: break;
        case NVSEMessagingInterface::kMessage_DeleteGame: break;
        case NVSEMessagingInterface::kMessage_RenameGame: break;
        case NVSEMessagingInterface::kMessage_RenameNewGame: break;
        case NVSEMessagingInterface::kMessage_NewGame: break;
        case NVSEMessagingInterface::kMessage_DeleteGameName: break;
        case NVSEMessagingInterface::kMessage_RenameGameName: break;
        case NVSEMessagingInterface::kMessage_RenameNewGameName: break;
        case NVSEMessagingInterface::kMessage_ClearScriptDataCache: break;
        case NVSEMessagingInterface::kMessage_MainGameLoop: break;
        case NVSEMessagingInterface::kMessage_ScriptCompile: break;
        case NVSEMessagingInterface::kMessage_EventListDestroyed: break;
        case NVSEMessagingInterface::kMessage_PostQueryPlugins: break;
        */

    case NVSEMessagingInterface::kMessage_DeferredInit:
        Console_Print("hNVSE version %.2f: Howdy neigh-bor!", (g_pluginVersion / 100.0F));
        break;
    default:
        break;
    }
}

extern "C" {

    __declspec(dllexport) bool NVSEPlugin_Query(const NVSEInterface* nvse, PluginInfo* info) {
        _MESSAGE("query");

        // Basic plugin info
        info->infoVersion = PluginInfo::kInfoVersion;
        info->name = "hNVSE";
        info->version = 2;

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
        _MESSAGE("load");

        g_pluginHandle = nvse->GetPluginHandle();

        g_nvseInterface = nvse; // NVSE interface

        // Script Commands
        nvse->SetOpcodeBase(0x3f40);
        REG_CMD(SetWorldspaceMapBounds);

        // Register to receive messages from NVSE
        g_messagingInterface = static_cast<NVSEMessagingInterface*>(nvse->QueryInterface(kInterface_Messaging));
        g_messagingInterface->RegisterListener(g_pluginHandle, "NVSE", MessageHandler);

        if (!nvse->isEditor) {
#if RUNTIME
            // script and function-related interfaces
            g_script = static_cast<NVSEScriptInterface*>(nvse->QueryInterface(kInterface_Script));
            g_stringInterface = static_cast<NVSEStringVarInterface*>(nvse->QueryInterface(kInterface_StringVar));
            g_arrayInterface = static_cast<NVSEArrayVarInterface*>(nvse->QueryInterface(kInterface_ArrayVar));
            g_dataInterface = static_cast<NVSEDataInterface*>(nvse->QueryInterface(kInterface_Data));
            g_eventInterface = static_cast<NVSEEventManagerInterface*>(nvse->QueryInterface(kInterface_EventManager));
            g_serializationInterface =
                static_cast<NVSESerializationInterface*>(nvse->QueryInterface(kInterface_Serialization));
            g_consoleInterface = static_cast<NVSEConsoleInterface*>(nvse->QueryInterface(kInterface_Console));
            ExtractArgsEx = g_script->ExtractArgsEx;

#if WantInventoryRefFunctions
            InventoryReferenceCreate = (_InventoryReferenceCreate)g_dataInterface->GetFunc(
                NVSEDataInterface::kNVSEData_InventoryReferenceCreate);
            InventoryReferenceGetForRefID = (_InventoryReferenceGetForRefID)g_dataInterface->GetFunc(
                NVSEDataInterface::kNVSEData_InventoryReferenceGetForRefID);
            InventoryReferenceGetRefBySelf = (_InventoryReferenceGetRefBySelf)g_dataInterface->GetFunc(
                NVSEDataInterface::kNVSEData_InventoryReferenceGetRefBySelf);
            InventoryReferenceCreateEntry = (_InventoryReferenceCreateEntry)g_dataInterface->GetFunc(
                NVSEDataInterface::kNVSEData_InventoryReferenceCreateEntry);
#endif

#if WantLambdaFunctions
            LambdaDeleteAllForScript = (_LambdaDeleteAllForScript)g_dataInterface->GetFunc(
                NVSEDataInterface::kNVSEData_LambdaDeleteAllForScript);
            LambdaSaveVariableList =
                (_LambdaSaveVariableList)g_dataInterface->GetFunc(NVSEDataInterface::kNVSEData_LambdaSaveVariableList);
            LambdaUnsaveVariableList = (_LambdaUnsaveVariableList)g_dataInterface->GetFunc(
                NVSEDataInterface::kNVSEData_LambdaUnsaveVariableList);
            IsScriptLambda = (_IsScriptLambda)g_dataInterface->GetFunc(NVSEDataInterface::kNVSEData_IsScriptLambda);
#endif

#if WantScriptFunctions
            HasScriptCommand =
                (_HasScriptCommand)g_dataInterface->GetFunc(NVSEDataInterface::kNVSEData_HasScriptCommand);
            DecompileScript = (_DecompileScript)g_dataInterface->GetFunc(NVSEDataInterface::kNVSEData_DecompileScript);
#endif

#endif
        }

        return true;
    }
};
