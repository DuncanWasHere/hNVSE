#include "nvse/GameAPI.h"
#include "nvse/PluginAPI.h"

IDebugLog    gLog("hNVSE.log");
PluginHandle g_pluginHandle = kPluginHandle_Invalid;

NVSEMessagingInterface    *g_messagingInterface{};
NVSEInterface             *g_nvseInterface{};
NVSECommandTableInterface *g_cmdTableInterface{};

// RUNTIME = Is not being compiled as a GECK plugin.
#if RUNTIME
NVSEScriptInterface        *g_script{};
NVSEStringVarInterface     *g_stringInterface{};
NVSEArrayVarInterface      *g_arrayInterface{};
NVSEDataInterface          *g_dataInterface{};
NVSESerializationInterface *g_serializationInterface{};
NVSEConsoleInterface       *g_consoleInterface{};
NVSEEventManagerInterface  *g_eventInterface{};
bool                        (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);

#define WantInventoryRefFunctions 0 // set to 1 if you want these PluginAPI functions
#if WantInventoryRefFunctions
_InventoryReferenceCreate       InventoryReferenceCreate{};
_InventoryReferenceGetForRefID  InventoryReferenceGetForRefID{};
_InventoryReferenceGetRefBySelf InventoryReferenceGetRefBySelf{};
_InventoryReferenceCreateEntry  InventoryReferenceCreateEntry{};
#endif

#define WantLambdaFunctions 0 // set to 1 if you want these PluginAPI functions
#if WantLambdaFunctions
_LambdaDeleteAllForScript LambdaDeleteAllForScript{};
_LambdaSaveVariableList   LambdaSaveVariableList{};
_LambdaUnsaveVariableList LambdaUnsaveVariableList{};
_IsScriptLambda           IsScriptLambda{};
#endif

#define WantScriptFunctions 0 // set to 1 if you want these PluginAPI functions
#if WantScriptFunctions
_HasScriptCommand HasScriptCommand{};
_DecompileScript  DecompileScript{};
#endif

#endif

/****************
 * Here we include the code + definitions for our script functions,
 * which are packed in header files to avoid lengthening this file.
 * Notice that these files don't require #include statements for globals/macros like ExtractArgsEx.
 * This is because the "fn_.h" files are only used here,
 * and they are included after such globals/macros have been defined.
 ***************/

// To Do: Make script functions

// Function Macros
#define REG_CMD(name) nvse->RegisterCommand(&kCommandInfo_##name) // Number return type
#define REG_TYPED_CMD(name, type) \
    nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_##type) // Non-number return types

// This is a message handler for nvse events
// With this, plugins can listen to messages such as whenever the game loads
void MessageHandler(NVSEMessagingInterface::Message *msg)
{
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
        // Make the plugin seem like it's doing something during runtime
        Console_Print("hNVSE version 1.0: Howdy neigh-bor!"); // Why bother making a variable for this yet
        break;
    default:
        break;
    }
}

// Everyone else is doing it
extern "C"
{

    __declspec(dllexport) bool NVSEPlugin_Query(const NVSEInterface *nvse, PluginInfo *info)
    {
        _MESSAGE("query");

        // fill out the info structure
        info->infoVersion = PluginInfo::kInfoVersion;
        info->name = "hNVSE";
        info->version = 1;

        // version checks
        if (nvse->nvseVersion < 0) {
            _ERROR("NVSE version too old (got %08X expected at least %08X)", nvse->nvseVersion, PACKED_NVSE_VERSION);
            return false;
        }

        if (!nvse->isEditor) {
            if (nvse->runtimeVersion < RUNTIME_VERSION_1_4_0_525) {
                _ERROR("incorrect runtime version (got %08X need at least %08X)", nvse->runtimeVersion,
                       RUNTIME_VERSION_1_4_0_525);
                return false;
            }

            if (nvse->isNogore) {
                _ERROR("NoGore is not supported");
                return false;
            }
        } else {
            if (nvse->editorVersion < CS_VERSION_1_4_0_518) {
                _ERROR("incorrect editor version (got %08X need at least %08X)", nvse->editorVersion,
                       CS_VERSION_1_4_0_518);
                return false;
            }
        }

        // version checks pass
        // any version compatibility checks should be done here
        return true;
    }

    __declspec(dllexport) bool NVSEPlugin_Load(NVSEInterface *nvse)
    {
        _MESSAGE("load");

        g_pluginHandle = nvse->GetPluginHandle();

        // save the NVSE interface in case we need it later
        g_nvseInterface = nvse;

        // register to receive messages from NVSE
        g_messagingInterface = static_cast<NVSEMessagingInterface *>(nvse->QueryInterface(kInterface_Messaging));
        g_messagingInterface->RegisterListener(g_pluginHandle, "NVSE", MessageHandler);

        if (!nvse->isEditor) {
#if RUNTIME
            // script and function-related interfaces
            g_script = static_cast<NVSEScriptInterface *>(nvse->QueryInterface(kInterface_Script));
            g_stringInterface = static_cast<NVSEStringVarInterface *>(nvse->QueryInterface(kInterface_StringVar));
            g_arrayInterface = static_cast<NVSEArrayVarInterface *>(nvse->QueryInterface(kInterface_ArrayVar));
            g_dataInterface = static_cast<NVSEDataInterface *>(nvse->QueryInterface(kInterface_Data));
            g_eventInterface = static_cast<NVSEEventManagerInterface *>(nvse->QueryInterface(kInterface_EventManager));
            g_serializationInterface =
                static_cast<NVSESerializationInterface *>(nvse->QueryInterface(kInterface_Serialization));
            g_consoleInterface = static_cast<NVSEConsoleInterface *>(nvse->QueryInterface(kInterface_Console));
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

        /*
         *
         * No commands yet... USELESS plugin!!
         *
         * nvse->SetOpcodeBase(0x4100);
         *
         */

        return true;
    }
};
