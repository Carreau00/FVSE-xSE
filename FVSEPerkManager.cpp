#include "FVSEPerkManager.h"

UInt8 playerWhaleRank;

namespace FVSEPerkManager {

	tArray<BGSPerk*> Perk_List;
	TESGlobal* perkPointGlobal;
	UInt32 CurrentLookupID = 1;

	UInt32 GetNextLookupID() {

		return CurrentLookupID++;
	}

	void ModPerkPoints(SInt32 value) {
		std::string PerkPointGlobalForm = MOD_FILE_EXTENSION;
		PerkPointGlobalForm.append(BASE_VORE_PERKGLOBAL);
		auto _PerkPointGlobal = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(PerkPointGlobalForm), TESForm, TESGlobal);

		SInt32 currentPerkPoints = _PerkPointGlobal->value;

		_PerkPointGlobal->value = currentPerkPoints + value;
	}

	void ProcessLists(BGSListForm* ListToProcess) {
		_MESSAGE("ListToProcess has %d forms", ListToProcess->forms.count);
		for (int i = 0; i < ListToProcess->forms.count; i++) {
			BGSPerk* PerkToAdd = DYNAMIC_CAST(ListToProcess->forms.entries[i], TESForm, BGSPerk);
			_MESSAGE("Adding perk %s", PerkToAdd->fullName.name.c_str());
			Perk_List.Push(PerkToAdd);
			
		}
	}

	void InitializePerks() {
		Perk_List.Clear();
		std::string BasePerksList = MOD_FILE_EXTENSION;
		BasePerksList.append(BASE_VORE_PERKLIST);
		auto _BasePerksList = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(BasePerksList), TESForm, BGSListForm);
		if (_BasePerksList == nullptr)
			_MESSAGE("BasePerksList not of BGS List type");
		else
			ProcessLists(_BasePerksList);
	}

	void RegisterAV(TESGlobal* perkGlobal) {
		perkPointGlobal = perkGlobal;
	}

	void ProcessConditions(PerkInformation* value, BGSPerk* aPerk, UInt8 rank, UInt8 rankCount) {
		PlayerCharacter* playerCharacter = (*g_player);
		Condition* tempCondition = aPerk->condition;

		while (tempCondition) {
			//We have the condition pulled up.  Now to grab the comparitor values  This will be done to set required values sent to menu as well as eligibility flags
			bool isEligible = false;
			bool isGetValue = false;
			bool allowByGender = true;
			UInt32 functionId = tempCondition->functionId;
			float compareValue = tempCondition->compareValue;
			TESForm * param1Form = tempCondition->param1.form;	//The AV that will be compared
			UInt32 u321 = tempCondition->param1.u32;
			UInt8 operation = tempCondition->comparisonType.op;
			UInt8 flags = tempCondition->comparisonType.flags;
			UInt8 playerSex = GetActorSex(playerCharacter);
			ActorValueInfo * conditionActorValue = nullptr;		//cast into this
			//These perks are all GetValue conditions.  Either with Pred/Prey level and with a SPECIAL stat
			switch (functionId) {
			case kFunction_GetBaseValue:
				isGetValue = true;
				conditionActorValue = DYNAMIC_CAST(param1Form, TESForm, ActorValueInfo);
				switch (operation) {
				case kCompareOp_Greater:
					isEligible = GetBaseValueAV(conditionActorValue, playerCharacter) > compareValue;
					break;
				case kCompareOp_GreaterEqual:
					isEligible = GetBaseValueAV(conditionActorValue, playerCharacter) >= compareValue;
					break;
				default:
					_MESSAGE("GetPerkInfo() %s had functionID %d.  operation not processed.", aPerk->fullName, functionId);
					break;
				}
				break;
			case kFunction_GetValue:
				isGetValue = true;
				conditionActorValue = DYNAMIC_CAST(param1Form, TESForm, ActorValueInfo);
				switch (operation) {
				case kCompareOp_Greater:
					isEligible = GetValueAV(conditionActorValue, playerCharacter) > compareValue;
					break;
				case kCompareOp_GreaterEqual:
					isEligible = GetValueAV(conditionActorValue, playerCharacter) >= compareValue;
					break;
				default:
					_MESSAGE("GetPerkInfo() %s had functionID %d.  operation not processed.", aPerk->fullName, functionId);
					break;
				}
				
				break;
			case kFunction_GetIsSex:
				allowByGender = (playerSex != (u321 ^ (int)compareValue ^ (operation != kCompareOp_Equal)));
				isEligible = allowByGender;
				break;
			}
			if (isGetValue) {
				switch (conditionActorValue->formID) {
				case kSpecialType_Strength:
					//break;
				case kSpecialType_Perception:
					//break;
				case kSpecialType_Endurance:
					//break;
				case kSpecialType_Charisma:
					//break;
				case kSpecialType_Intelligence:
					//break;
				case kSpecialType_Agility:
					//break;
				case kSpecialType_Luck:
					value->SpecialType = conditionActorValue->formID;
					value->SpecialReq = (UInt32)compareValue;
					break;
				default:
					std::string PredLevelAVForm = MOD_FILE_EXTENSION;
					PredLevelAVForm.append(BASE_VORE_PREDLEVELAV);

					std::string PreyLevelAVForm = MOD_FILE_EXTENSION;
					PreyLevelAVForm.append(BASE_VORE_PREYLEVELAV);

					if (conditionActorValue->formID == FVUtilities::GetFormIDFromIdentifier(PredLevelAVForm)) {
						value->VoreType = (UInt32)filterFlag_predType;
						value->VoreReq.Push(tempCondition->compareValue);
					}
					else if (conditionActorValue->formID == FVUtilities::GetFormIDFromIdentifier(PreyLevelAVForm)) {
						value->VoreType = (UInt32)filterFlag_preyType;
						value->VoreReq.Push(tempCondition->compareValue);
					}
					break;
				}
			}
			//we need to set eligibility false if the player doesn't meet a requirement of the perk.  Eligilibility should only directly affect the rank we want to set for purchase
			if (rank == rankCount && !isEligible) {
				value->isEligible = isEligible;
			}
			if (!allowByGender)
				value->allowByGender = allowByGender;

			tempCondition = tempCondition->next;
			
		}
		
	}

	PerkInformation ProcessBasePerk(BGSPerk* basePerk, PerkInformation* Result) {
		PlayerCharacter* playerCharacter = (*g_player);
		PerkInformation InfoResult;
		if (basePerk == nullptr) {
			_MESSAGE("ProcessBasePerk received null basePerk");
			//return;
			return InfoResult;
		}
		BGSPerk* tempPerk = basePerk;
		UInt8 rank = 0;
		//UInt8 rankCount = 0;
		UInt8 numRanks = basePerk->numRanks;
		BSString tempDescription;
		
		for(UInt8 rankCount=1; rankCount <= numRanks; rankCount++) {
			InfoResult.descriptionHolder.Push(tempPerk);
			if (!HasPerk(*g_player, tempPerk) && rank == 0) {
				rank = rankCount;
				InfoResult.Perk = tempPerk;
				InfoResult.formID = tempPerk->formID;
				InfoResult.swfPath = tempPerk->swfPath;
			}
			else if (HasPerk(*g_player, tempPerk) && tempPerk->fullName.name.c_str() == "Belly of the Whale") {
				playerWhaleRank = rankCount;
			}
			Condition* tempCondition = tempPerk->condition;
			ProcessConditions(&InfoResult, tempPerk, rank, rankCount);
			tempPerk = tempPerk->nextPerk;
		}
		//process all conditions before setting filters
		switch (InfoResult.VoreType) {
		case filterFlag_predType:
			InfoResult.filterFlag |= filterFlag_predType;
			break;
		case filterFlag_preyType:
			InfoResult.filterFlag |= filterFlag_preyType;
			break;
		}
		InfoResult.filterFlag |= ((InfoResult.isEligible ? 1 : 2) << 4);
		InfoResult.rank = rank;
		InfoResult.numRanks = numRanks;

		return InfoResult;
	}

	void ProcessPerks(GFxValue * dst, GFxMovieRoot * root) {
		for (int i = 0; i < Perk_List.count; i++)
		{
			BGSPerk * basePerk = Perk_List.entries[i];
			//all perks in this list are assumed to be base rank.  If higher ranks are passed in, we end up with duplicates
			
			PerkInformation ProcessedPerk = ProcessBasePerk(basePerk, &ProcessedPerk);
			
			if (ProcessedPerk.Perk != nullptr && ProcessedPerk.allowByGender)
			{
				GFxValue PerkToPopulate;

				root->CreateObject(&PerkToPopulate);
				RegisterString(&PerkToPopulate, root, "text", ProcessedPerk.Perk->fullName.name.c_str());
				Register(&PerkToPopulate, "formID", ProcessedPerk.formID);
				Register(&PerkToPopulate, "disabled", !ProcessedPerk.isEligible);		//flip the bool.  We want to use Disabled as a way to toggle perk selection alphas during menu transitions
				Register(&PerkToPopulate, "rank", ProcessedPerk.rank);
				Register(&PerkToPopulate, "maxRank", ProcessedPerk.numRanks);
				
				RegisterString(&PerkToPopulate, root, "description", FVUtilities::GetDescription(basePerk).c_str());
				
				GFxValue fxDescArray;
				root->CreateArray(&fxDescArray);

				for (int j = 0; j < ProcessedPerk.descriptionHolder.count; j++) {
					GFxValue fxValue;
					BGSPerk* tempPerk = ProcessedPerk.descriptionHolder.entries[j];
					root->CreateString(&fxValue, FVUtilities::GetDescription(tempPerk).c_str());
					//fxArray.SetMember("text", &fxValue);
					fxDescArray.PushBack(&fxValue);
				}
				PerkToPopulate.SetMember("descriptions", &fxDescArray);
				
				Register(&PerkToPopulate, "specialType", ProcessedPerk.SpecialType);
				
				Register(&PerkToPopulate, "specialReq", ProcessedPerk.SpecialReq);
				Register(&PerkToPopulate, "voreType", ProcessedPerk.VoreType);
				GFxValue fxVoreValArray;
				root->CreateArray(&fxVoreValArray);
				for (int k = 0; k < ProcessedPerk.VoreReq.count; k++) {
					GFxValue fxValue;
					float tempVoreValue = ProcessedPerk.VoreReq.entries[k];
					fxValue.SetUInt((UInt32)tempVoreValue);
					fxVoreValArray.PushBack(&fxValue);
				}
				PerkToPopulate.SetMember("voreReqValues", &fxVoreValArray);
				RegisterString(&PerkToPopulate, root, "SWFFile", ProcessedPerk.swfPath.c_str());
				Register(&PerkToPopulate, "filterFlag", ProcessedPerk.filterFlag);
				
				dst->PushBack(&PerkToPopulate);
				
			}
		}
		_MESSAGE("End of ProcessPerks.  Proccessed %d perks", dst->GetArraySize());
	}
}