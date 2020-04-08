#pragma once
#include "GlobalDefs.h"
#include "FVSEScaleform.h"
#include "f4se/GameForms.h"
#include "f4se/GameFormComponents.h"
#include "f4se/GameReferences.h"
#include "f4se/GameRTTI.h"
#include "f4se/ScaleformMovie.h"
#include "f4se/ScaleformValue.h"
#include "RVA/RVA.h"
#include <time.h>




namespace FVUtilities {

	template <typename T>
	T GetOffset(const void* baseObject, int offset)
	{
		return *reinterpret_cast<T*>((uintptr_t)baseObject + offset);
	}

	BSFixedString GetDescription(TESForm *thisForm);
	TESForm * GetFormFromIdentifier(const std::string & identifier);
	UInt32 GetFormIDFromIdentifier(const std::string & identifier);
	void UpdateRVAAddress();

};


void PopulatePerkEntry(GFxValue * dst, GFxMovieRoot * root, BGSPerk * perk);
void ModValueAV(ActorValueInfo* actorValue, Actor* actor, float delta);
float GetBaseValueAV(ActorValueInfo* actorValue, Actor* actor);
float GetValueAV(ActorValueInfo* actorValue, Actor* actor);
float GetAVBaseValueByFormID(int formID, Actor * actor);
UInt8 GetActorSex(Actor * actor);

void CycleMenu_int(BSFixedString MenuName, bool open);

typedef UInt8(*_HasPerk)(Actor* actor, BGSPerk* perk);
extern RVA<_HasPerk> HasPerk;
typedef void(*_AddPerk)(Actor* actor, BGSPerk* perk, UInt8 rank);
extern RVA<_AddPerk> AddPerk;
extern RVA <void*> g_UIManager;
typedef void(*_UI_AddMessage)(void* menuManager, BSFixedString* menuName, unsigned int menuAction);
extern RVA <_UI_AddMessage> UI_AddMessage;

template<typename T>
inline void Register(GFxValue *dst, const char *name, T value)
{
}

template<>
inline void Register(GFxValue *dst, const char *name, UInt32 value)
{
	GFxValue fxValue;
	fxValue.SetUInt(value);
	dst->SetMember(name, &fxValue);
}

template<>
inline void Register(GFxValue *dst, const char *name, UInt8 value)
{
	GFxValue fxValue;
	fxValue.SetUInt(value);
	dst->SetMember(name, &fxValue);
}

template<>
inline void Register(GFxValue *dst, const char *name, int value)
{
	GFxValue fxValue;
	fxValue.SetInt(value);
	dst->SetMember(name, &fxValue);
}

template<>
inline void Register(GFxValue *dst, const char *name, bool value)
{
	GFxValue fxValue;
	fxValue.SetBool(value);
	dst->SetMember(name, &fxValue);
}

inline void RegisterString(GFxValue *dst, GFxMovieRoot *root, const char *name, const char *value) {
	GFxValue fxValue;
	root->CreateString(&fxValue, value);
	dst->SetMember(name, &fxValue);
}

inline void RegisterStringArray(GFxValue *dst, GFxMovieRoot *root, const char *name, tArray<const char*> *value) {
	const char **arrayString=nullptr;
	*arrayString = new const char[value->count];
	for (int i = 0; i < value->count; i++) {
		arrayString[i] = value->entries[i];
	}

	GFxValue fxValue;
	root->CreateString(&fxValue, *arrayString);
	dst->SetMember(name, &fxValue);
}

inline void RegisterStringArrayNew(GFxValue *dst, GFxMovieRoot *root, const char *name, tArray<BSString> *value) {
	GFxValue* fxArray;
	GFxValue fxValue;
	root->CreateArray(fxArray);

	for (int i = 0; i < value->count; i++) {
		root->CreateString(&fxValue, value->entries[i].Get());
		fxArray->PushBack(&fxValue);
	}
	dst->SetMember(name, fxArray);
}