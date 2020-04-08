//#include "main.h"
#include "common/IDirectoryIterator.h"
#include "f4se/GameForms.h"
#include "f4se/GameTypes.h"
#include "f4se/GameSettings.h"
#include "f4se/PluginAPI.h"
#include "f4se/ScaleformLoader.h"

#include "f4se_common/f4se_version.h"

#include "FVSEPapyrus.h"
#include "FVSEPipboyStats.h"
#include "FVSEScaleform.h"
#include "FVSETranslator.h"
#include "GlobalDefs.h"

#include <ShlObj.h>

std::string extensionNameShort = "FVSE";
UInt32 modVersion = 1;

IDebugLog						gLog;

PluginHandle					g_pluginHandle		= kPluginHandle_Invalid;

F4SEPapyrusInterface			*g_papyrus			= NULL;
F4SEScaleformInterface			*g_scaleform		= NULL;
F4SEMessagingInterface			*g_messaging		= NULL;
F4SESerializationInterface		*g_serialization	= NULL;

//	----------------------
//	REGISTRATION FUNCTIONS
//	----------------------

bool RegisterPapyrusNativeFunctions(VirtualMachine* vm) 
{
	//Add native papyrus registration code
	return FVPapyrus::RegisterPapyrusFunctions(vm);
	
}

void OnF4SEMessage(F4SEMessagingInterface::Message* msg) {
	switch (msg->type) {
	case F4SEMessagingInterface::kMessage_GameDataReady:
		FVSEPerkManager::InitializePerks();
		FVSEPipboyStats::InitializeStatLists();
		break;
	case F4SEMessagingInterface::kMessage_GameLoaded:
		BSScaleformTranslator* translator = (BSScaleformTranslator*)(*g_scaleformManager)->stateBag->GetStateAddRef(GFxState::kInterface_Translator);
		if (translator) {
			FVSETranslator::LoadTranslations(translator);
		}
		break;
	}
}

//	----------------------
//	F4SE API FUNCTIONS
//	----------------------
extern "C"
{

	bool F4SEPlugin_Query(const F4SEInterface * f4se, PluginInfo * info)
	{
		
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4\\F4SE\\FVSE.log");
		_MESSAGE("FVSE Log Initialized");

		// populate info structure
		info->infoVersion	=		PluginInfo::kInfoVersion;
		info->name			=		extensionNameShort.c_str();
		info->version		=		modVersion;

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = f4se->GetPluginHandle();

		// Check game version
		if (f4se->runtimeVersion != CURRENT_FALLOUT_RUNTIME) {
			char str[512];
			sprintf_s(str, sizeof(str), "Your game version: v%d.%d.%d.%d\nExpected version: v%d.%d.%d.%d\n%s will be disabled.",
				GET_EXE_VERSION_MAJOR(f4se->runtimeVersion),
				GET_EXE_VERSION_MINOR(f4se->runtimeVersion),
				GET_EXE_VERSION_BUILD(f4se->runtimeVersion),
				GET_EXE_VERSION_SUB(f4se->runtimeVersion),
				GET_EXE_VERSION_MAJOR(CURRENT_FALLOUT_RUNTIME),
				GET_EXE_VERSION_MINOR(CURRENT_FALLOUT_RUNTIME),
				GET_EXE_VERSION_BUILD(CURRENT_FALLOUT_RUNTIME),
				GET_EXE_VERSION_SUB(CURRENT_FALLOUT_RUNTIME),
				extensionNameShort.c_str()
			);

			MessageBox(NULL, str, extensionNameShort.c_str(), MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		// get the papyrus interface and query its version
		g_papyrus = (F4SEPapyrusInterface *)f4se->QueryInterface(kInterface_Papyrus);
		if (!g_papyrus)
		{
			_MESSAGE("couldn't get papyrus interface");
			return false;
		}
		g_scaleform = (F4SEScaleformInterface *)f4se->QueryInterface(kInterface_Scaleform);
		if (!g_scaleform)
		{
			_MESSAGE("couldn't get scaleform interface");
			return false;
		}
		// Get the messaging interface
		g_messaging = (F4SEMessagingInterface *)f4se->QueryInterface(kInterface_Messaging);
		if (!g_messaging) {
			_MESSAGE("couldn't get messaging interface");
			return false;
		}
		// Get the serialization interface
		g_serialization = (F4SESerializationInterface *)f4se->QueryInterface(kInterface_Serialization);
		if (!g_serialization) {
			_MESSAGE("couldn't get serialization interface");
			return false;
		}
		_MESSAGE("F4SEPlugin_Query complete.");
		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface *f4se)
	{
		_MESSAGE("Fallout Vore FVSE Load.");
		
		g_serialization->SetUniqueID(g_pluginHandle, 'FVSE');
		//g_serialization->SetRevertCallback(g_pluginHandle, FVSESerialization::RevertCallback);
		//g_serialization->SetLoadCallback(g_pluginHandle, FVSESerialization::LoadCallback);
		//g_serialization->SetSaveCallback(g_pluginHandle, FVSESerialization::SaveCallback);
		
		FVUtilities::UpdateRVAAddress();
		RVAManager::UpdateAddresses(f4se->runtimeVersion);
		g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage);
		if (g_papyrus) {
			g_papyrus->Register(RegisterPapyrusNativeFunctions);
			_MESSAGE("Papyrus Register Succeeded");
		}
		if (g_scaleform) {
			g_scaleform->Register(extensionNameShort.c_str(), FVScaleform::RegisterScaleform);
			_MESSAGE("Scaleform Register Succeeded");
		}
		return true;
	}

};