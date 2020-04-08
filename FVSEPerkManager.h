#pragma once

#include "f4se/ScaleformValue.h"
#include "f4se/ScaleformMovie.h"
#include "f4se/GameForms.h"

#include "FVUtilities.h"

#include "GlobalDefs.h"

extern UInt8 playerWhaleRank;

namespace FVSEPerkManager {
	
	

	struct PerkInformation
	{
		BGSPerk*				Perk = nullptr;
		UInt32					formID = 0;
		tArray<const char*>*	descriptions;
		tArray<BGSPerk*>		descriptionHolder;
		UInt8					rank = 0;
		UInt8					numRanks = 0;
		UInt32					SpecialType = 0;
		UInt32					SpecialReq = 0;
		UInt32					VoreType = 0;
		tArray<float>			VoreReq;
		BSFixedString			swfPath;
		bool					isEligible = true;
		bool					allowByGender = true;
		int						filterFlag = 0;
		
	};

	void ModPerkPoints(SInt32 value);
	void ProcessLists(BGSListForm* ListToProcess);
	void InitializePerks();
	void ProcessPerks(GFxValue * dst, GFxMovieRoot * root);
	PerkInformation ProcessBasePerk(BGSPerk* basePerk, PerkInformation* InfoResult);
	void RegisterAV(TESGlobal* perkAV);


}