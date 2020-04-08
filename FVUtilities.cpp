#include "FVUtilities.h"

#include "f4se/GameData.h"
#include "f4se/GameMenus.h"

RVA <_HasPerk> HasPerk;
RVA <_AddPerk> AddPerk;
RVA <_UI_AddMessage> UI_AddMessage;
RVA <void*> g_UIManager;
std::string LastMenuOpened;

namespace FVUtilities
{
	BSFixedString GetDescription(TESForm *thisForm) {
		if (!thisForm)
			return BSFixedString();
		TESDescription * temp = DYNAMIC_CAST(thisForm, TESForm, TESDescription);
		if (temp) {
			BSString str;
			CALL_MEMBER_FN(temp, Get)(&str, nullptr);
			return str.Get();
		}
		return BSFixedString();
	}

	TESForm * GetFormFromIdentifier(const std::string & identifier)
	{
		auto delimiter = identifier.find('|');
		if (delimiter != std::string::npos)
		{
			std::string modName = identifier.substr(0, delimiter);
			std::string modForm = identifier.substr(delimiter + 1);
			const ModInfo* mod = (*g_dataHandler)->LookupModByName(modName.c_str());
			if (mod && mod->modIndex != -1)
			{
				UInt32 formID = std::stoul(modForm, nullptr, 16) & 0xFFFFFF;
				UInt32 flags = GetOffset<UInt32>(mod, 0x334);
				if (flags & (1 << 9))
				{
					formID &= 0xFFF;
					formID |= 0xFE;
					formID |= GetOffset<UInt16>(mod, 0x372) << 12;
				}
				else
				{
					formID |= (mod->modIndex) << 24;
				}
				return LookupFormByID(formID);
			}
		}
		return nullptr;
	}

	UInt32 GetFormIDFromIdentifier(const std::string & identifier) {
		auto delimiter = identifier.find('|');
		if (delimiter != std::string::npos)
		{
			std::string modName = identifier.substr(0, delimiter);
			std::string modForm = identifier.substr(delimiter + 1);
			const ModInfo* mod = (*g_dataHandler)->LookupModByName(modName.c_str());
			if (mod && mod->modIndex != -1)
			{
				UInt32 formID = std::stoul(modForm, nullptr, 16) & 0xFFFFFF;
				UInt32 flags = GetOffset<UInt32>(mod, 0x334);
				if (flags & (1 << 9))
				{
					formID &= 0xFFF;
					formID |= 0xFE;
					formID |= GetOffset<UInt16>(mod, 0x372) << 12;
				}
				else
				{
					formID |= (mod->modIndex) << 24;
				}
				return formID;
			}
		}
		return 0;
	}

};

	void ModValueAV(ActorValueInfo* actorValue, Actor* actor,  float delta)
	{
		actor->actorValueOwner.Mod(0, actorValue, delta);
	}

	float GetValueAV(ActorValueInfo* actorValue, Actor* actor)
	{
		return actor->actorValueOwner.GetValue(actorValue);
	}

	float GetBaseValueAV(ActorValueInfo* actorValue, Actor* actor)
	{
		return actor->actorValueOwner.GetBase(actorValue);
	}

	void PopulatePerkEntry(GFxValue * dst, GFxMovieRoot * root, BGSPerk * perk) {

	}

	float GetAVBaseValueByFormID(int formID, Actor * actor) {
		float value = 0.0;
		TESForm * form = LookupFormByID(formID);
		if (form && form->formType == ActorValueInfo::kTypeID) {
			return GetBaseValueAV((ActorValueInfo *)form, actor);
		}
		return value;
	}

	float GetAVValueByFormID(int formID, Actor * actor) {
		float value = 0.0;
		TESForm * form = LookupFormByID(formID);
		if (form && form->formType == ActorValueInfo::kTypeID) {
			return GetValueAV((ActorValueInfo *)form, actor);
		}
		return value;
	}

	UInt8 GetActorSex(Actor * actor) {
		UInt8 gender = 0;
		TESNPC * actorBase = DYNAMIC_CAST(actor->baseForm, TESForm, TESNPC);
		if (actorBase) {
			gender = CALL_MEMBER_FN(actorBase, GetSex)();
		}
		return gender;
	}

	

	void CycleMenu_int(BSFixedString MenuName, bool open) {
		BSFixedString hudMenu("HUDMenu");
		BSFixedString pipboyMenu("PipboyMenu");
		int CycleAction = open ? 1 : 3;
		int OppositeAction = !open ? 1 : 3;
		/*if ((*g_ui)->IsMenuOpen(hudMenu)) {
			UI_AddMessage(*g_UIManager, &hudMenu, OppositeAction);
			LastMenuOpened = hudMenu.c_str();
		}*/
		/*if ((*g_ui)->IsMenuOpen(pipboyMenu)) {
			//UI_AddMessage(*g_UIManager, &pipboyMenu, OppositeAction);  //Likely, the best course of action would be to set ButtonHintBar_mc of PipboyMenu to false if it doesn't go invis
			IMenu *menu = (*g_ui)->GetMenu(pipboyMenu);
			auto root = menu->movie->movieRoot;
			if (root && open) {
				/*GFxValue val;
				
				GFxValue testReturn;

				root->GetVariable(&testReturn, "root.Menu_mc.ButtonHintBar_mc.alpha");
				_MESSAGE("ButtonHintBar alpha before Set %f Number", testReturn.GetNumber());
				_MESSAGE("ButtonHintBar alpha before Set %f Int", testReturn.GetInt());
				_MESSAGE("ButtonHintBar alpha before Set %f UInt", testReturn.GetUInt());
				_MESSAGE("ButtonHintBar alpha before Set %d Bool", testReturn.GetBool());
								
				val.SetNumber(0.0);
				//val.SetBool(false);
				bool DisabledBar = root->SetVariable("root.Menu_mc.ButtonHintBar_mc.alpha", &val);
				
				
				root->GetVariable(&testReturn, "root.Menu_mc.ButtonHintBar_mc.alpha");
				_MESSAGE("ButtonHintBar alpha after Set %f Number", testReturn.GetNumber());
				_MESSAGE("ButtonHintBar alpha after Set %f Int", testReturn.GetInt());
				_MESSAGE("ButtonHintBar alpha after Set %f UInt", testReturn.GetUInt());
				_MESSAGE("ButtonHintBar alpha after Set %d Bool", testReturn.GetBool());

				val.SetBool(false);
				root->SetVariable("root.Menu_mc.enabled", &val);
				_MESSAGE("DisabledBar %d shows root_SetVariable of alpha to 1 successful", DisabledBar);
				if (!DisabledBar)
					_MESSAGE("failed to disable button hint bar in PipboyMenu.");
				}
			}

			LastMenuOpened = pipboyMenu.c_str();
		}*/
		
		//UI_AddMessage(*g_UIManager, &BSFixedString("VignetteMenu"), CycleAction);
		UI_AddMessage(*g_UIManager, &MenuName, CycleAction);

		if (!open) {
			//We're closing the level up menu.  Cycle the last menu opened back on
			BSFixedString LastMenu(LastMenuOpened.c_str());
			//if (LastMenuOpened == "PipboyMenu") {
				IMenu *menu = (*g_ui)->GetMenu(pipboyMenu);
				auto root = menu->movie->movieRoot;
				if (root) {
					/*GFxValue val;
					val.SetNumber(0.0);
					root->SetVariable("root.Menu_mc.ButtonHintBar_mc.alpha", &val);
					val.SetBool(true);
					root->SetVariable("root.Menu_mc.enabled", &val);*/
					FVScaleform::UpdatePipboyValues(root);
				}
			//}
			//else {
			//	UI_AddMessage(*g_UIManager, &LastMenu, OppositeAction);
			//}
			LastMenuOpened = "";
		}
	}

	void FVUtilities::UpdateRVAAddress()
	{
		HasPerk = RVA <_HasPerk>(
			"HasPerk", {
				{ RUNTIME_VERSION_1_10_163, 0x00DA6600 },
				{ RUNTIME_VERSION_1_10_130, 0x00DA64E0 },
			}, "48 83 EC 28 48 8B 81 00 03 00 00 48 85 C0 74 16 4C 8B C2");
		AddPerk = RVA <_AddPerk>(
			"AddPerk", {
				{ RUNTIME_VERSION_1_10_163, 0x00DA6200 },
				{ RUNTIME_VERSION_1_10_130, 0x00DA60E0 },
			}, "48 89 5C 24 08 48 89 6C 24 10 56 57 41 56 48 83 EC 40 48 8D 99 34 04 00 00");
		UI_AddMessage = RVA <_UI_AddMessage>(
			"UI_AddMessage", {
				{ RUNTIME_VERSION_1_10_163, 0x0204CB90 },
				{ RUNTIME_VERSION_1_10_130, 0x0204CA70 },
			}, "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 48 89 7C 24 20 41 54 41 56 41 57 48 83 EC 20 44 8B 0D ? ? ? ? 65 48 8B 04 25 58 00 00 00 48 8B E9 4A 8B 34 C8 B9 C0 09 00 00");
		g_UIManager = RVA <void*>(
			"g_UIManager", {
				{ RUNTIME_VERSION_1_10_163, 0x058D0AC8 },
				{ RUNTIME_VERSION_1_10_130, 0x058D0AC8 },
			}, "48 8B 3D ? ? ? ? E8 ? ? ? ? 41 B8 01 00 00 00 48 8B CF 48 8B D0", 0, 3, 7);
	}
