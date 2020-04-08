#pragma once
struct StaticFunctionTag;
class VirtualMachine;

namespace FVPapyrus 
{
	bool RegisterPapyrusFunctions(VirtualMachine* vm);
}