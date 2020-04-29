#include "FVSEScaleform.h"

std::string MenuNameString = "LevelUpVore";
std::string LastMenuOpen;

UInt32 playerCurrWeight;
UInt32 playerMaxWeight;

//----------------------
//	AS3 Callback Classes
//----------------------

class Scaleform_OpenPerksMenu : public GFxFunctionHandler {
public:
	virtual void Invoke(Args* args) {
		args->result->SetBool(FVScaleform::OpenVoreLevelMenu());
	}
};

class Scaleform_CloseVoreLevelMenu : public GFxFunctionHandler {
public:
	virtual void Invoke(Args* args) {
		args->result->SetBool(FVScaleform::CloseVoreLevelMenu());
	}
};

class Scaleform_RegisterInputDevice : public GFxFunctionHandler {
public:
	virtual void Invoke(Args* args) {
		BSFixedString MenuName(MenuNameString.c_str());
		if ((*g_ui)->IsMenuOpen(MenuName)) {
			_MESSAGE("Scaleform_RegisterInputDevice called.");
			args->result->SetBool((*g_inputDeviceMgr)->IsGamepadEnabled());
		}
	}
};

class Scaleform_AddPredLevel : public GFxFunctionHandler {
public:
	virtual void Invoke(Args* args) {
		BSFixedString MenuName(MenuNameString.c_str());
		if ((*g_ui)->IsMenuOpen(MenuName)) {
			std::string PredLevelAVForm = MOD_FILE_EXTENSION;
			PredLevelAVForm.append(BASE_VORE_PREDLEVELAV);
			auto _PredLevelAVForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(PredLevelAVForm), TESForm, ActorValueInfo);

			ModValueAV(_PredLevelAVForm, *g_player, 1);
			
			std::string LevelPointGlobalForm = MOD_FILE_EXTENSION;
			LevelPointGlobalForm.append(BASE_VORE_LEVELPOINTGLOBAL);
			auto _LevelPointGlobalForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(LevelPointGlobalForm), TESForm, TESGlobal);
			_LevelPointGlobalForm->value -= 1;

			args->result->SetBool(true);
			if (FVScaleform::UpdateMenu())
				_MESSAGE("Scaleform_AddPredLevel update menu successful");
		}
	}
};

class Scaleform_AddPreyLevel : public GFxFunctionHandler {
public:
	virtual void Invoke(Args* args) {
		BSFixedString MenuName(MenuNameString.c_str());
		if ((*g_ui)->IsMenuOpen(MenuName)) {
			std::string PreyLevelAVForm = MOD_FILE_EXTENSION;
			PreyLevelAVForm.append(BASE_VORE_PREYLEVELAV);
			auto _PreyLevelAVForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(PreyLevelAVForm), TESForm, ActorValueInfo);

			ModValueAV(_PreyLevelAVForm, *g_player, 1);

			std::string LevelPointGlobalForm = MOD_FILE_EXTENSION;
			LevelPointGlobalForm.append(BASE_VORE_LEVELPOINTGLOBAL);
			auto _LevelPointGlobalForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(LevelPointGlobalForm), TESForm, TESGlobal);
			_LevelPointGlobalForm->value -= 1;

			args->result->SetBool(true);
			if (FVScaleform::UpdateMenu())
				_MESSAGE("Scaleform_AddPreyLevel update menu successful");
		}
	}
};

class Scaleform_AddPerk : public GFxFunctionHandler {
public:
	virtual void Invoke(Args* args) {
		TESForm * perkForm = LookupFormByID(args->args[0].GetUInt());
		BGSPerk * perkToAdd = DYNAMIC_CAST(perkForm, TESForm, BGSPerk);

		if (perkToAdd) {
			AddPerk(*g_player, perkToAdd, 0);
			args->result->SetBool(true);
			FVSEPerkManager::ModPerkPoints(-1);
		}
		if (FVScaleform::UpdateMenu())
			_MESSAGE("Scaleform_AddPerk update menu successful");
	}
};

class Scaleform_PlayUISound : public GFxFunctionHandler {
public:
	virtual void	Invoke(Args * args) {
		if (args->args[0].GetType() != GFxValue::kType_String) return;
		PlayUISound(args->args[0].GetString());
	}
};

class Scaleform_SwallowItem : public GFxFunctionHandler {
public:
	virtual void	Invoke(Args * args) {
		args->result->SetBool(false);
		BSFixedString functionName("PlayerSwallowItem");
		std::string questIDString = MOD_FILE_EXTENSION;
		questIDString.append(SCAT_QUEST_ID);
		TESForm * questForm = FVUtilities::GetFormFromIdentifier(questIDString);
		VirtualMachine* vm = (*g_gameVM)->m_virtualMachine;
		GFxValue testValue = args->args[0];

		UInt32 itemFormID = testValue.data.u32;
		if (itemFormID != 0){
			VMValue passedArg;
			VMValue::ArrayData* arrayData = nullptr;
			vm->CreateArray(&passedArg, 1, &arrayData);
			passedArg.type.value = VMValue::kType_VariableArray;
			passedArg.data.arr = arrayData;
			VMValue* var = new VMValue;
			PlatformAdapter::ConvertScaleformValue(var, &args->args[0], vm);
			arrayData->arr.entries[0].SetVariable(var);
			
			UInt64 handle = vm->GetHandlePolicy()->Create(questForm->formType, questForm);
			VMIdentifier* identifier = nullptr;
			const char* scriptName = "ScriptObject";
			vm->GetObjectIdentifier(handle, scriptName, 1, &identifier, 0);
			if (identifier) {
				CallFunctionNoWait_Internal(vm, 0, identifier, &functionName, &passedArg);
				args->result->SetBool(true);
			}
		}
		
	}
};

class Scaleform_UpdateContainerValues : public GFxFunctionHandler {
public:
	virtual void	Invoke(Args * args) {
		FVScaleform::UpdateBellyContainer(args->movie->movieRoot);
	}
};

class Scaleform_UpdatePipboyValues : public GFxFunctionHandler {
public:
	virtual void	Invoke(Args * args) {
		FVScaleform::UpdatePipboyValues(args->movie->movieRoot);
	}
};

namespace FVScaleform
{
	bool RegisterCustomMenu() //BSFixedString menuName, BSFixedString menuPath, BSFixedString rootPath, MenuData menuData)
	{
		BSFixedString menuName(MenuNameString.c_str());
		BSFixedString menuPath("LevelUpVore");
		BSFixedString rootPath("root1");
		
		if (!(*g_ui)->IsMenuRegistered(menuName))
		{
			
			BSReadAndWriteLocker locker(&g_customMenuLock);
			g_customMenuData[menuName.c_str()].menuPath = menuPath;
			g_customMenuData[menuName.c_str()].rootPath = rootPath;
			g_customMenuData[menuName.c_str()].menuFlags = 0x400805;
			g_customMenuData[menuName.c_str()].movieFlags = 0x03;
			g_customMenuData[menuName.c_str()].extFlags = 0x03;
			g_customMenuData[menuName.c_str()].depth = 0x06;

			(*g_ui)->Register(menuName.c_str(), CreateCustomMenu);
			_MESSAGE("Registering %s", menuName.c_str());
			_MESSAGE("menuPath %s", g_customMenuData[menuName.c_str()].menuPath.c_str());
			_MESSAGE("rootPath %s", g_customMenuData[menuName.c_str()].rootPath.c_str());
			_MESSAGE("menuFlags 0x%08X", g_customMenuData[menuName.c_str()].menuFlags);
			_MESSAGE("movieFlags 0x%08X", g_customMenuData[menuName.c_str()].movieFlags);
			_MESSAGE("extFlags 0x%08X", g_customMenuData[menuName.c_str()].extFlags);
			_MESSAGE("depth 0x%08X", g_customMenuData[menuName.c_str()].depth);

			return true;
		}

		return false;
	}

	void GetActorInfo(GFxValue * dst, Actor* actor, bool isActive) {
		PlayerCharacter* pPlayerCharacter = (*g_player);
		BSFixedString tempName = CALL_MEMBER_FN(actor, GetReferenceName)();
		std::string activeName = tempName.c_str();
		if (isActive) {
			activeName.append(" (Active)");
		}
		BSFixedString fullName(activeName.c_str());
		//grab the global value
		std::string PerkPointGlobalForm = MOD_FILE_EXTENSION;
		PerkPointGlobalForm.append(BASE_VORE_PERKGLOBAL);
		auto _PerkPointGlobal = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(PerkPointGlobalForm), TESForm, TESGlobal);
		
		std::string PreyLevelAVForm = MOD_FILE_EXTENSION;
		PreyLevelAVForm.append(BASE_VORE_PREYLEVELAV);
		auto _PreyLevelAVForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(PreyLevelAVForm), TESForm, ActorValueInfo);

		std::string PredLevelAVForm = MOD_FILE_EXTENSION;
		PredLevelAVForm.append(BASE_VORE_PREDLEVELAV);
		auto _PredLevelAVForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(PredLevelAVForm), TESForm, ActorValueInfo);

		std::string LevelPointGlobalForm = MOD_FILE_EXTENSION;
		LevelPointGlobalForm.append(BASE_VORE_LEVELPOINTGLOBAL);
		auto _LevelPointGlobalForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(LevelPointGlobalForm), TESForm, TESGlobal);

		std::string VoreXPAVForm = MOD_FILE_EXTENSION;
		VoreXPAVForm.append(BASE_VORE_XPAV);
		auto _VoreXPAVForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(VoreXPAVForm), TESForm, ActorValueInfo);

		std::string CapacityPointGlobalForm = MOD_FILE_EXTENSION;
		CapacityPointGlobalForm.append(BASE_VORE_PLAYERCAPACITYPOINTGLOBAL);
		auto _CapacityPointGlobalForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(CapacityPointGlobalForm), TESForm, TESGlobal);

		std::string CapacityAVForm = MOD_FILE_EXTENSION;
		CapacityAVForm.append(BASE_VORE_CAPACITYAV);
		auto _CapacityAVForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(CapacityAVForm), TESForm, ActorValueInfo);

		std::string CurrentPreyAVForm = MOD_FILE_EXTENSION;
		CurrentPreyAVForm.append(BASE_VORE_CURRENTPREYAV);
		auto _CurrentPreyAVForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(CurrentPreyAVForm), TESForm, ActorValueInfo);

		std::string CurrWeightPointGlobalForm = MOD_FILE_EXTENSION;
		CurrWeightPointGlobalForm.append(BASE_VORE_CURRENT_WEIGHT);
		auto _CurrWeightPointGlobalForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(CurrWeightPointGlobalForm), TESForm, TESGlobal);

		std::string MaxWeightPointGlobalForm = MOD_FILE_EXTENSION;
		MaxWeightPointGlobalForm.append(BASE_VORE_MAX_WEIGHT);
		auto _MaxWeightPointGlobalForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(MaxWeightPointGlobalForm), TESForm, TESGlobal);

		//grab player specific information
		int _PreyLevel = (int)GetValueAV(_PreyLevelAVForm, pPlayerCharacter);
		int _PredLevel = (int)GetValueAV(_PredLevelAVForm, pPlayerCharacter);
		int playerLevelAvailable = (int)_LevelPointGlobalForm->value;
		double _VoreXP = (double)GetValueAV(_VoreXPAVForm, pPlayerCharacter);
		int _CapacityBase = (int)GetBaseValueAV(_CapacityAVForm, pPlayerCharacter);
		double reqXP = ceil(pow((_PreyLevel + _PredLevel + playerLevelAvailable + 13)/2, 1.7));
		double playerCapacityPoints = (double)_CapacityPointGlobalForm->value;
		double reqCap = ceil(pow((_CapacityBase - playerWhaleRank), 2.25));
		
		if (_VoreXP > reqXP) {
			_MESSAGE("Vore XP greater than required XP  Setting current equal to required to prevent UI error - current: %f requried: %f", _VoreXP, reqXP);
			_VoreXP = reqXP;
		}
		GFxValue statData;
		statData.SetInt((int)GetAVBaseValueByFormID(kSpecialType_Strength, actor));
		dst->PushBack(&statData);
		statData.SetInt((int)GetAVBaseValueByFormID(kSpecialType_Perception, actor));
		dst->PushBack(&statData);
		statData.SetInt((int)GetAVBaseValueByFormID(kSpecialType_Endurance, actor));
		dst->PushBack(&statData);
		statData.SetInt((int)GetAVBaseValueByFormID(kSpecialType_Charisma, actor));
		dst->PushBack(&statData);
		statData.SetInt((int)GetAVBaseValueByFormID(kSpecialType_Intelligence, actor));
		dst->PushBack(&statData);
		statData.SetInt((int)GetAVBaseValueByFormID(kSpecialType_Agility, actor));
		dst->PushBack(&statData);
		statData.SetInt((int)GetAVBaseValueByFormID(kSpecialType_Luck, actor));
		dst->PushBack(&statData);
		statData.SetInt((int)_PerkPointGlobal->value);
		dst->PushBack(&statData);
		statData.SetInt((int)GetValueAV(_PreyLevelAVForm, actor));
		dst->PushBack(&statData);
		statData.SetInt((int)GetValueAV(_PredLevelAVForm, actor));
		dst->PushBack(&statData);
		statData.SetInt(playerLevelAvailable);
		dst->PushBack(&statData);
		statData.SetNumber(_VoreXP);
		dst->PushBack(&statData);
		statData.SetNumber(reqXP);
		dst->PushBack(&statData);
		statData.SetNumber(playerCapacityPoints);
		dst->PushBack(&statData);
		statData.SetNumber(reqCap);
		dst->PushBack(&statData);
		statData.SetNumber((int)GetBaseValueAV(_CapacityAVForm, actor));
		dst->PushBack(&statData);
		statData.SetNumber((int)GetValueAV(_CapacityAVForm, actor));
		dst->PushBack(&statData);
		statData.SetNumber((int)GetValueAV(_CurrentPreyAVForm, actor));
		dst->PushBack(&statData);
		statData.SetString(fullName);
		dst->PushBack(&statData);
		statData.SetNumber((int)_MaxWeightPointGlobalForm->value);
		dst->PushBack(&statData);
		statData.SetNumber((int)_CurrWeightPointGlobalForm->value);
		dst->PushBack(&statData);
	}

	bool UpdateMenu_int(GFxMovieRoot * root)  //add the passed variables
	{
		if (root) {
			playerWhaleRank = 0;
			GFxValue arrArgs[3];
			root->CreateArray(&arrArgs[0]);
			FVSEPerkManager::ProcessPerks(&arrArgs[0], root);
			root->CreateArray(&arrArgs[1]);
			GetActorInfo(&arrArgs[1], (*g_player), false);
			arrArgs[2].SetBool((*g_inputDeviceMgr)->IsGamepadEnabled());
			root->Invoke("root.Menu_mc.BeginMenu", nullptr, arrArgs, 3);
			return true;
		}
		return false;
	}

	bool UpdateMenu() {
		BSFixedString MenuName(MenuNameString.c_str());
		IMenu* menu = (*g_ui)->GetMenu(MenuName);
		if (!menu)
			return false;
		GFxMovieView* movieView = menu->movie;
		if (!movieView)
			return false;
		GFxMovieRoot* movieRoot = movieView->movieRoot;
		if (movieRoot) {
			UpdateMenu_int(movieRoot);
			return true;
		}
		return false;
	}

	bool OpenVoreLevelMenu()
	{
		BSFixedString MenuName(MenuNameString.c_str());
		if ((*g_ui)->IsMenuRegistered(MenuName)) {
			CycleMenu_int(MenuName, true);
			RegisterForInput(true);
			return true;
		}
		return false;
	}

	bool CloseVoreLevelMenu()
	{
		BSFixedString MenuName(MenuNameString.c_str());
		if ((*g_ui)->IsMenuRegistered(MenuName)) {
			
			CycleMenu_int(MenuName, false);
			return true;
		}
		return false;
	}
	
	void UpdateBellyContainer_int(GFxMovieRoot* root) {
		if (root) {
			_MESSAGE("UpdateBellyContainer_int called %d aCurrWeight %d aMaxWeight", playerCurrWeight, playerMaxWeight);
			GFxValue args[2];
			args[0].SetUInt(playerCurrWeight);
			args[1].SetUInt(playerMaxWeight);
			root->Invoke("root.BellyContainer_loader.content.UpdateEncumbrance", nullptr, args, 2);
		}
	}

	void UpdateBellyContainer(GFxMovieRoot* root) {
		UpdateBellyContainer_int(root);
	}

	void BellyContainerUpdate(UInt32 aCurrWeight, UInt32 aMaxWeight) {
		IMenu* menu = (*g_ui)->GetMenu(BSFixedString("ContainerMenu"));
		if (!menu)
			return;
		auto movie = menu->movie;
		if (!movie)
			return;
		auto movieRoot = movie->movieRoot;
		if (!movieRoot)
			return;
		playerCurrWeight = aCurrWeight;
		playerMaxWeight = aMaxWeight;
		UpdateBellyContainer_int(movieRoot);
	}

	void OpenedBellyContainer(UInt32 aCurrWeight, UInt32 aMaxWeight) {
		IMenu* menu = (*g_ui)->GetMenu(BSFixedString("ContainerMenu"));
		if (!menu)
			return;
		auto movie = menu->movie;
		if (!movie)
			return;
		auto movieRoot = movie->movieRoot;
		if (!movieRoot)
			return;
		GFxValue loader, urlRequest, root;
		movieRoot->GetVariable(&root, "root");
		movieRoot->CreateObject(&loader, "flash.display.Loader");
		movieRoot->CreateObject(&urlRequest, "flash.net.URLRequest", &GFxValue("ContainerBellyInject.swf"), 1);
		root.SetMember("BellyContainer_loader", &loader);
		GFxValue FVSECodeObj; movieRoot->CreateObject(&FVSECodeObj);
		root.SetMember("FVSECodeObj", &FVSECodeObj);
		RegisterFunction<Scaleform_UpdateContainerValues>(&FVSECodeObj, movieRoot, "UpdateContainerValues");
		movieRoot->Invoke("root.BellyContainer_loader.load", nullptr, &urlRequest, 1);
		movieRoot->Invoke("root.addChild", nullptr, &loader, 1);
		playerCurrWeight = aCurrWeight;
		playerMaxWeight = aMaxWeight;
	}

	void UpdatePipboyValues(GFxMovieRoot* root) {
		if (!root) {
			IMenu* menu = (*g_ui)->GetMenu(BSFixedString("PipboyMenu"));
			if (!menu)
				return;
			auto movie = menu->movie;
			if (!movie)
				return;
			auto root = movie->movieRoot;
			if (!root)
				return;
		}

		PlayerCharacter* pPlayerCharacter = (*g_player);
		
		GFxValue arrArgs[3];
		FVSEPipboyStats::ProcessPipboyStats(arrArgs, root, pPlayerCharacter);
		arrArgs[2].SetBool((*g_inputDeviceMgr)->IsGamepadEnabled());
		_MESSAGE("UpdatePipboyValues sending F4SECodeUpdate call");
		root->Invoke("root.PipboyVore_loader.content.F4SECodeUpdate", nullptr, arrArgs, 3);
	}

	void RegisterBGSCodeObj(GFxMovieRoot* movieRoot) {
		IMenu* menu = (*g_ui)->GetMenu(BSFixedString("PipboyMenu"));
		if (!menu)
			return;
		auto movie = menu->movie;
		if (!movie)
			return;
		auto root = movie->movieRoot;
		if (!root)
			return;
		GFxValue pipBGSCodeObj;
		root->GetVariable(&pipBGSCodeObj, "root.Menu_mc.BGSCodeObj");
		if (!(pipBGSCodeObj.IsUndefined())) {
			_MESSAGE("Sending pipboy BGSCode Object up to perks menu");
			movieRoot->Invoke("root.Menu_mc.RegisterCodeObj", nullptr, &pipBGSCodeObj, 1);
		}
	}

	bool RegisterScaleform(GFxMovieView * view, GFxValue * f4se_root)
	{
		
		GFxMovieRoot *movieRoot = view->movieRoot;
		GFxValue currentSWFPath;
		std::string currentSWFPathString = "";

		RegisterFunction<Scaleform_AddPerk>(f4se_root, movieRoot, "AddPerk");
		RegisterFunction<Scaleform_CloseVoreLevelMenu>(f4se_root, movieRoot, "CloseLevelVoreMenu");
		RegisterFunction<Scaleform_RegisterInputDevice>(f4se_root, movieRoot, "RegisterInputDevice");
		RegisterFunction<Scaleform_AddPredLevel>(f4se_root, movieRoot, "AddPredLevel");
		RegisterFunction<Scaleform_AddPreyLevel>(f4se_root, movieRoot, "AddPreyLevel");
		RegisterFunction<Scaleform_PlayUISound>(f4se_root, view->movieRoot, "PlayUISound");
		
		if (movieRoot->GetVariable(&currentSWFPath, "root.loaderInfo.url")) {
			currentSWFPathString = currentSWFPath.GetString();
			_MESSAGE("%s was opened", currentSWFPathString.c_str());
		}
		else
			_MESSAGE("WARNING: Scaleform registration failed.");
		if (currentSWFPathString.find("LevelUpVore.swf") != std::string::npos)
		{			
			if (!movieRoot)
				return false;
			_MESSAGE("Initializing Fallout Vore Level Menu");
			//Let's update the perks for view
			UpdateMenu_int(movieRoot);
		}
		else if (currentSWFPathString.find("PipboyMenu.swf") != std::string::npos) {
						
			if (!movieRoot)
				return false;

			_MESSAGE("Detected Pipboy menu.  Allowing input calls to open level up menu");
			
			GFxValue loader, urlRequest, root;
			movieRoot->GetVariable(&root, "root");
			movieRoot->CreateObject(&loader, "flash.display.Loader");
			movieRoot->CreateObject(&urlRequest, "flash.net.URLRequest", &GFxValue("PipboyVoreTab.swf"), 1);
			root.SetMember("PipboyVore_loader", &loader);
			GFxValue FVSECodeObj; movieRoot->CreateObject(&FVSECodeObj);
			root.SetMember("FVSECodeObj", &FVSECodeObj);
			
			movieRoot->Invoke("root.PipboyVore_loader.load", nullptr, &urlRequest, 1);
			
			movieRoot->Invoke("root.addChild", nullptr, &loader, 1);

			RegisterFunction<Scaleform_OpenPerksMenu>(&FVSECodeObj, view->movieRoot, "OpenPerksMenu");
			RegisterFunction<Scaleform_SwallowItem>(&FVSECodeObj, view->movieRoot, "SwallowItem");
			RegisterFunction<Scaleform_UpdatePipboyValues>(&FVSECodeObj, view->movieRoot, "UpdatePipboyValues");

			RegisterForInput(true);

		}
		else if (currentSWFPathString.find("HUDMenu.swf") != std::string::npos) {
			auto root = view->movieRoot;

			if (!root)
				return false;
			_MESSAGE("Player returned to main window.  Disabling input calls if they were already enabled.");
			RegisterForInput(false);
		}
		
		return true;
		
	}

	//--------------
	// Input Handler
	//--------------

	class F4SEInputHandler : public BSInputEventUser
	{
	public:
		F4SEInputHandler() : BSInputEventUser(true) { }

		virtual void OnButtonEvent(ButtonEvent * inputEvent) {
			UInt32 keyCode;
			UInt32 deviceType = inputEvent->deviceType;
			UInt32 keyMask = inputEvent->keyMask;
			UInt32 eventType = inputEvent->eventType;

			if (deviceType == InputEvent::kDeviceType_Mouse) {
				// Mouse
				keyCode = InputMap::kMacro_MouseButtonOffset + keyMask;
			}
			else if (deviceType == InputEvent::kDeviceType_Gamepad) {
				// Gamepad
				keyCode = InputMap::GamepadMaskToKeycode(keyMask);
			}
			else {
				keyCode = keyMask;
			}

			BSFixedString* ControlID = inputEvent->GetControlID();
			BSFixedString MenuName(MenuNameString.c_str());

			//mask whether the control event is a down press (isDown coincident with timer at 0) or if it's an up press (!isDown after the timer has moved from 0 indicating a key lift)
			float timer = inputEvent->timer;
			bool isDown = inputEvent->isDown == 1.0f && timer == 0.0f;
			bool isUp = inputEvent->isDown == 0.0f && timer != 0.0f;
			bool keyDown;
			//_MESSAGE("eventType: %d keyCode: %X deviceType: %X ControlID: %s", eventType, keyCode, deviceType, ControlID->c_str());

			if ((*g_ui)->IsMenuOpen(MenuNameString.c_str())) {
				if (isDown) {
					keyDown = true;
				}
				else if (isUp) {
					keyDown = false;
				}
				if(isDown || isUp){
					IMenu* menu = (*g_ui)->GetMenu(MenuName);
					GFxMovieRoot* movieRoot = menu->movie->movieRoot;
					GFxValue args[3];
					args[0].SetString(ControlID->c_str());
					args[1].SetBool(isDown);
					args[2].SetInt(keyCode);
					_MESSAGE("Sending user event to level up menu");
					movieRoot->Invoke("root.Menu_mc.ProcessUserInput", nullptr, args, 3);
				
				}
			}
			else if ((*g_ui)->IsMenuOpen("PipboyMenu")) {
				_MESSAGE("PipboyMenu eventType: %d keyCode: %X deviceType: %X ControlID: %s", eventType, keyCode, deviceType, ControlID->c_str());
				if (isDown) {
					keyDown = true;
				}
				else if (isUp) {
					keyDown = false;
				}
				if (isDown || isUp) {
					IMenu* menu = (*g_ui)->GetMenu(BSFixedString("PipboyMenu"));
					GFxMovieRoot* movieRoot = menu->movie->movieRoot;
					GFxValue args[3];
					args[0].SetString(ControlID->c_str());
					args[1].SetBool(isDown);
					args[2].SetInt(keyCode);
					movieRoot->Invoke("root.PipboyVore_loader.content.ProcessUserInputVore", nullptr, args, 3);

				}
			}
		}
		virtual void OnThumbstickEvent(ThumbstickEvent * inputEvent) {
			
			BSFixedString* ControlID = inputEvent->GetControlID();
			//_MESSAGE("unk20[0]: %u unk20[1]: %u unk20[2]: %u unk20[3]: %u unk20[4]: %u unk20[5]: %u ControlID: %s", inputEvent->unk20[0], inputEvent->unk20[1], inputEvent->unk20[2], inputEvent->unk20[3], inputEvent->unk20[4], inputEvent->unk20[5], ControlID->c_str());
			UInt32 prevDir = inputEvent->unk20[4];
			UInt32 currDir = inputEvent->unk20[5];
			if ((*g_ui)->IsMenuOpen(MenuNameString.c_str())) {
				BSFixedString MenuName(MenuNameString.c_str());
				IMenu* menu = (*g_ui)->GetMenu(MenuName);
				GFxMovieRoot* movieRoot = menu->movie->movieRoot;
				GFxValue args[3];
				if (prevDir == 0 && currDir == 2)
					args[0].SetString("StrafeRight");
				else if (prevDir == 0 && currDir == 4)
					args[0].SetString("StrafeLeft");
				else
					args[0].SetString("NONE");

				args[1].SetBool(prevDir == 0 || prevDir == currDir);
				args[2].SetInt(0xFF);
				movieRoot->Invoke("root.Menu_mc.ProcessUserInput", nullptr, args, 3);
			}
		}
	};

	F4SEInputHandler g_scaleformInputHandler;

	void RegisterForInput(bool bRegister) {
		if (bRegister) {
			g_scaleformInputHandler.enabled = true;
			tArray<BSInputEventUser*>* inputEvents = &((*g_menuControls)->inputEvents);
			BSInputEventUser* inputHandler = &g_scaleformInputHandler;
			int idx = inputEvents->GetItemIndex(inputHandler);
			if (idx == -1) {
				inputEvents->Push(&g_scaleformInputHandler);
				_MESSAGE("Registered for input events");
			}
		}
		else {
			g_scaleformInputHandler.enabled = false;
		}
	}
};