#include "FVSEPapyrus.h"
#include "FVSEScaleform.h"
#include "GlobalDefs.h"
#include "FVSEPerkManager.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

namespace FVPapyrus
{
	bool OpenVoreLevelMenu(StaticFunctionTag *)
	{
		return FVScaleform::OpenVoreLevelMenu();

	}

	bool CloseVoreLevelMenu(StaticFunctionTag *)
	{
		return FVScaleform::CloseVoreLevelMenu();
	}

	void AddPerkList(StaticFunctionTag *base, BGSListForm *PerkList)
	{
		_MESSAGE("AddPerkList called.  Perklist %d", PerkList->formType);
		//FVSEPerkManager::ProcessLists(PerkList);
	}

	void LoadBellyContainer(StaticFunctionTag*, UInt32 aCurrWeight, UInt32 aMaxWeight) {
		FVScaleform::OpenedBellyContainer(aCurrWeight, aMaxWeight);
	}

	void BellyItemChanged(StaticFunctionTag*, UInt32 aCurrWeight, UInt32 aMaxWeight) {
		FVScaleform::BellyContainerUpdate(aCurrWeight, aMaxWeight);
	}
}

bool FVPapyrus::RegisterPapyrusFunctions(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, bool>("OpenVoreLevelMenu", FALLOUT_VORE, FVPapyrus::OpenVoreLevelMenu, vm));
	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, bool>("CloseVoreLevelMenu", FALLOUT_VORE, FVPapyrus::CloseVoreLevelMenu, vm));
	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, void, BGSListForm*>("AddPerkList", FALLOUT_VORE, FVPapyrus::AddPerkList, vm));
	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, UInt32, UInt32>("LoadBellyContainer", FALLOUT_VORE, FVPapyrus::LoadBellyContainer, vm));
	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, UInt32, UInt32>("BellyItemChanged", FALLOUT_VORE, FVPapyrus::BellyItemChanged, vm));
	return true;
}