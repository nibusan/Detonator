#pragma once
#include"../Object/Gun/GunType.h"
#include"../Object/Grenade/GrenadeType.h"

struct LOADOUT
{
	GUN_TYPE gunType_;
	GRENADE_TYPE greType_[2];
};
