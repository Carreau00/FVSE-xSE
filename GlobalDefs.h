#pragma once
#include "f4se_common/f4se_version.h"

#define CURRENT_FALLOUT_RUNTIME CURRENT_RELEASE_RUNTIME

#define FALLOUT_VORE "FalloutVore"
#define MENU_NAME_STRING "LevelUpVore"
#define MOD_FILE_EXTENSION "FalloutVore.esp"
#define BASE_VORE_PERKLIST "|FA1C"	//pipe is here to allow ease of appending strings
#define BASE_VORE_PERKGLOBAL "|F99C"
#define BASE_VORE_PREDLEVELAV "|1023B"
#define BASE_VORE_PREDLEVELAV_INT 0x1023B
#define BASE_VORE_PREYLEVELAV "|F98F"
#define BASE_VORE_PREYLEVELAV_INT 0xF98F
#define BASE_VORE_LEVELPOINTGLOBAL "|1023A"
#define BASE_VORE_XPAV "|F990"
#define BASE_VORE_PLAYERCAPACITYPOINTGLOBAL "|7A99"
#define BASE_VORE_CAPACITYAV "|F97C"
#define BASE_VORE_CURRENTPREYAV "|F97F"
#define SCAT_QUEST_ID "|BD2E"
#define BASE_VORE_PREYSTATLIST "|27D5"
#define BASE_VORE_PREDSTATLIST "|27D6"
#define BASE_VORE_DATASTATLIST "|27E0"
#define BASE_VORE_ACTIVECOMPANIONLIST "|6B08"
#define BASE_VORE_INACTIVECOMPANIONLIST "|6B09"
#define BASE_VORE_DIGEST_SPEED_GLOBAL "|F9A3"
#define BASE_VORE_DIGEST_SPEED_AV "|AA12"

enum Functions {				//these are copied from GameFormComponents in the condition class.  Only required values have been enumerated
	kFunction_GetValue = 14,
	kFunction_GetIsSex = 70,
	kFunction_GetBaseValue = 277
};

enum CompareOperators {
	kCompareOp_Equal = 0,
	kCompareOp_NotEqual,
	kCompareOp_Greater,
	kCompareOp_GreaterEqual,
	kCompareOp_Less,
	kCompareOp_LessEqual
};

enum VoreLevelType {
	kLevelType_Pred = 0,
	kLevelType_Prey
};

enum SpecialType {				//the hardcoded AVs for SPECIAL stats are also being used to enumerate the type of stat
	kSpecialType_Strength = 706,
	kSpecialType_Perception,
	kSpecialType_Endurance,
	kSpecialType_Charisma,
	kSpecialType_Intelligence,
	kSpecialType_Agility,
	kSpecialType_Luck
};

enum FilterFlags {
	filterFlag_elig = 1,
	filterFlag_notelig = 2,
	filterFlag_predType = 4,
	filterFlag_preyType = 8
};