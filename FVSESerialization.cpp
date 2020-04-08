#include "FVSESerialization.h"

namespace FVSESerialization
{
	void RevertCallback(const F4SESerializationInterface * intfc)
	{
		_DMESSAGE("Clearing Fallout Vore serialization data.");

	}

	void LoadCallback(const F4SESerializationInterface * intfc)
	{
		_DMESSAGE("Loading Fallout Vore serialization data.");
		UInt32 type, version, length;
		bool error = false;


	}

	void SaveCallback(const F4SESerializationInterface * intfc)
	{

	}
}