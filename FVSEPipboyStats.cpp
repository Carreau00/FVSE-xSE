#include "FVSEPipboyStats.h"

namespace FVSEPipboyStats {

	tArray<ActorValueInfo*> PredStat_List;
	tArray<ActorValueInfo*> PreyStat_List;
	tArray<ActorValueInfo*> DataStat_List;

	void ProcessStatList(BGSListForm* ListToProcess, tArray<ActorValueInfo*>* ListToAdd) {
		for (int i = 0; i < ListToProcess->forms.count; i++) {
			ActorValueInfo* AVtoAdd = DYNAMIC_CAST(ListToProcess->forms.entries[i], TESForm, ActorValueInfo);
			ListToAdd->Push(AVtoAdd);
		}
	}

	void InitializeStatLists() {
		_MESSAGE("FVSE Initialize Stat Lists");
		PredStat_List.Clear();
		std::string BasePredStatList = MOD_FILE_EXTENSION;
		BasePredStatList.append(BASE_VORE_PREDSTATLIST);
		auto _BasePredStatList = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(BasePredStatList), TESForm, BGSListForm);
		if (_BasePredStatList == nullptr)
			_MESSAGE("BasePredStatList not of BGS List type");
		else 
			ProcessStatList(_BasePredStatList, &PredStat_List);
		
		PreyStat_List.Clear();
		std::string BasePreyStatList = MOD_FILE_EXTENSION;
		BasePreyStatList.append(BASE_VORE_PREYSTATLIST);
		auto _BasePreyStatList = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(BasePreyStatList), TESForm, BGSListForm);
		if (_BasePreyStatList == nullptr)
			_MESSAGE("BasePreyStatList not of BGS List type");
		else 
			ProcessStatList(_BasePreyStatList, &PreyStat_List);
		
		DataStat_List.Clear();
		std::string BaseDataStatList = MOD_FILE_EXTENSION;
		BaseDataStatList.append(BASE_VORE_DATASTATLIST);
		auto _BaseDataStatList = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(BaseDataStatList), TESForm, BGSListForm);
		if (_BaseDataStatList == nullptr)
			_MESSAGE("BaseDataStatList not of BGS List type");
		else
			ProcessStatList(_BaseDataStatList, &DataStat_List);
	}

	void ProcessPredStats(GFxValue* dst, GFxMovieRoot * root, Actor* actor) {
		PlayerCharacter* pPlayerCharacter = (*g_player);
		std::string DigestSpeedAV = MOD_FILE_EXTENSION;
		DigestSpeedAV.append(BASE_VORE_DIGEST_SPEED_AV);
		auto _digestSpeedAV = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(DigestSpeedAV), TESForm, ActorValueInfo);
		for (int i = 0; i < PredStat_List.count; i++) {
			ActorValueInfo* baseAV = PredStat_List.entries[i];
			GFxValue AVToPopulate;
			root->CreateObject(&AVToPopulate);
			RegisterString(&AVToPopulate, root, "text", baseAV->fullName.name.c_str());
			if (baseAV == _digestSpeedAV) {
				std::string DigestSpeedGlobal = MOD_FILE_EXTENSION;
				DigestSpeedGlobal.append(BASE_VORE_DIGEST_SPEED_GLOBAL);
				auto _digestSpeedGlobal = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(DigestSpeedGlobal), TESForm, TESGlobal);
				int _digestSpeed = _digestSpeedGlobal->value - (int)GetValueAV(baseAV, actor);
				if (_digestSpeed < 1)
					_digestSpeed = 1;
				Register(&AVToPopulate, "value", _digestSpeed);
			}
			else
				Register(&AVToPopulate, "value", (int)GetValueAV(baseAV, actor));
			RegisterString(&AVToPopulate, root, "description", FVUtilities::GetDescription(baseAV).c_str());
			dst->PushBack(&AVToPopulate);
		}
	}

	void ProcessPreyStats(GFxValue* dst, GFxMovieRoot * root, Actor* actor) {
		
		for (int i = 0; i < PreyStat_List.count; i++) {
			ActorValueInfo* baseAV = PreyStat_List.entries[i];
			GFxValue AVToPopulate;
			root->CreateObject(&AVToPopulate);
			RegisterString(&AVToPopulate, root, "text", baseAV->fullName.name.c_str());
			Register(&AVToPopulate, "value", (int)GetValueAV(baseAV, actor));
			RegisterString(&AVToPopulate, root, "description", FVUtilities::GetDescription(baseAV).c_str());
			dst->PushBack(&AVToPopulate);
		}
	}

	void ProcessPlayerStats(GFxValue* dst, GFxMovieRoot* root) {
		PlayerCharacter* pPlayerCharacter = (*g_player);
		GFxValue PlayerStats;
		root->CreateObject(&PlayerStats);
		RegisterString(&PlayerStats, root, "text", "$Vore");
		GFxValue StatValues;
		root->CreateArray(&StatValues);
		for (int i = 0; i < DataStat_List.count; i++) {
			ActorValueInfo* statAV = DataStat_List.entries[i];
			GFxValue value;
			root->CreateObject(&value);
			RegisterString(&value, root, "text", statAV->fullName.name.c_str());
			Register(&value, "value", (int)GetValueAV(statAV, pPlayerCharacter));
			StatValues.PushBack(&value);
		}
		PlayerStats.SetMember("statArray", &StatValues);
		dst->PushBack(&PlayerStats);
	}

	void ProcessSingleActorStat(GFxValue* dst, GFxMovieRoot* root, Actor* actor, bool isActive = false){
		std::string PredLevelAVForm = MOD_FILE_EXTENSION;
		PredLevelAVForm.append(BASE_VORE_PREDLEVELAV);
		auto _PredLevelAVForm = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(PredLevelAVForm), TESForm, ActorValueInfo);
		int _PredLevel = (int)GetValueAV(_PredLevelAVForm, actor);
		
		GFxValue ActorStats;
		root->CreateArray(&ActorStats);
		GFxValue PredPreyStats;
		root->CreateArray(&PredPreyStats);
		if (_PredLevel > 0)
			ProcessPredStats(&PredPreyStats, root, actor);

		ProcessPreyStats(&PredPreyStats, root, actor);
		ActorStats.PushBack(&PredPreyStats);
		GFxValue ActorInfo;
		root->CreateArray(&ActorInfo);
		FVScaleform::GetActorInfo(&ActorInfo, actor, isActive);
		ActorStats.PushBack(&ActorInfo);
		dst->PushBack(&ActorStats);
	}
	
	void ProcessPipboyStats(GFxValue* dst, GFxMovieRoot* root, Actor* actor) {
		root->CreateArray(&dst[0]);
		
		PlayerCharacter* pPlayerCharacter = (*g_player);
		ProcessSingleActorStat(&dst[0], root, pPlayerCharacter);

		std::string ActiveCompanionList = MOD_FILE_EXTENSION;
		ActiveCompanionList.append(BASE_VORE_ACTIVECOMPANIONLIST);
		auto _ActiveCompanionList = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(ActiveCompanionList), TESForm, BGSListForm);
		if (_ActiveCompanionList->tempForms != nullptr) {
			for (int i = 0; i < _ActiveCompanionList->tempForms->count; i++) {
				Actor* companion = DYNAMIC_CAST(LookupFormByID(_ActiveCompanionList->tempForms->entries[i]), TESForm, Actor);
				ProcessSingleActorStat(&dst[0], root, companion, true);
			}
		}
		std::string InactiveCompanionList = MOD_FILE_EXTENSION;
		InactiveCompanionList.append(BASE_VORE_INACTIVECOMPANIONLIST);
		auto _InactiveCompanionList = DYNAMIC_CAST(FVUtilities::GetFormFromIdentifier(InactiveCompanionList), TESForm, BGSListForm);
		if (_InactiveCompanionList->tempForms != nullptr) {
			for (int i = 0; i < _InactiveCompanionList->tempForms->count; i++) {
				Actor* companion = DYNAMIC_CAST(LookupFormByID(_InactiveCompanionList->tempForms->entries[i]), TESForm, Actor);
				ProcessSingleActorStat(&dst[0], root, companion);
			}
		}
		root->CreateArray(&dst[1]);
		ProcessPlayerStats(&dst[1], root);
	}
}