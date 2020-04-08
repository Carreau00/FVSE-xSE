#pragma once

#include "f4se/ScaleformValue.h"
#include "f4se/ScaleformMovie.h"
#include "f4se/ScaleformCallbacks.h"
#include "f4se/GameData.h"
#include "f4se/GameRTTI.h"
#include "f4se/GameMenus.h"
#include "f4se/CustomMenu.h"
#include "f4se/PapyrusEvents.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/PapyrusScaleformAdapter.h"
#include "f4se/PapyrusUI.h"
#include "f4se/InputMap.h"
#include "FVUtilities.h"
#include "GlobalDefs.h"
#include "FVSEPerkManager.h"
#include "FVSEPipboyStats.h"

namespace FVScaleform
{
	bool RegisterCustomMenu(); // BSFixedString menuName, BSFixedString menuPath, BSFixedString rootPath); //StaticFunctionTag *,  was first parameter originally
	void GetActorInfo(GFxValue * dst, Actor* actor, bool isActive);
	bool UpdateMenu();
	void RegisterForInput(bool bRegister);
	bool OpenVoreLevelMenu();
	bool CloseVoreLevelMenu();
	void UpdateBellyContainer(GFxMovieRoot* root);
	void BellyContainerUpdate(UInt32 aCurrWeight, UInt32 aMaxWeight);
	void OpenedBellyContainer(UInt32 aCurrWeight, UInt32 aMaxWeight);
	void UpdatePipboyValues(GFxMovieRoot* root);
	bool RegisterScaleform(GFxMovieView * view, GFxValue * f4se_root);
}
