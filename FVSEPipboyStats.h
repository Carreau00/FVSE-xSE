#pragma once
#include "f4se/ScaleformValue.h"
#include "FVUtilities.h"
#include "GlobalDefs.h"

namespace FVSEPipboyStats {
	void InitializeStatLists();
	void ProcessPredStats(GFxValue* dst, GFxMovieRoot * root, Actor* actor);
	void ProcessPreyStats(GFxValue* dst, GFxMovieRoot * root, Actor* actor);
	void ProcessPlayerStats(GFxValue* dst, GFxMovieRoot * root);
	void ProcessPipboyStats(GFxValue* dst, GFxMovieRoot* root, Actor* actor);
}