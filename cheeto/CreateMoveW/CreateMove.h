#include "../aim/AimBot.h"

//Entity List
int EntityCount = 0;
struct Entity { 
	int EntID;
	bool Visible;
};
Entity EntityCache[1000];
void ProcessEntityCache(UserCmd* Cmd)
{
	//setup global data
	LPlayer* LP_Ent = LP; int CurCount = 80;
	if (!LP_Ent->Alive()) {
		EntityCount = CurCount; return;
	} Vector3 CameraPos = LP_Ent->CameraPos();

	//process entities
	for (int i = 0; i < 80; i++)
	{
		auto CurEnt = Ent(i);
		if (CurEnt->Type(Player))
		{
			//clamp distance
			Vector3 Head3D = CurEnt->HitBoxPos(0, false);
			if ((Math::GameDist(CameraPos, Head3D)) > Visuals::DistanceESP)
				goto InvalidEnt;

			//save entity
			EntityCache[i].EntID = i;
			EntityCache[i].Visible = CurEnt->VisiblePos(LP_Ent, Head3D);
		} 

		else { //skip
			InvalidEnt: 
			EntityCache[i].EntID = -1;
		}
	}

	//wait update tick
	static BYTE Tick = 0;
	if (Visuals::LootESP && !Tick++)
	{
		//process loot
		for (int i = 0; (i < 10000); i++)
		{
			auto CurEnt = Ent(i);
			if (CurEnt->Type(LootEnt))
			{
				//clamp distance
				Vector3 RootPos = CurEnt->AbsOrgin();
				if (Math::GameDist(CameraPos, RootPos) > Visuals::DistanceLoot)
					continue;

				//save entity
				if (CurCount == 999) break;
				EntityCache[CurCount].EntID = i;
				EntityCache[CurCount].Visible = 1;
				CurCount++;
			}
		}

		//update full count
		EntityCount = CurCount;
	}
}

//store original function
typedef void(__fastcall* CreateMoveFn)(DWORD64, int, float, bool);
CreateMoveFn CreateMoveOrg = nullptr;

//CreateMove Hook (indx 4)
void __fastcall CreateMoveHk(DWORD64 thisPtr, int SeqNum, float SampleTime, bool Active)
{
	//no spread
	
	/*if (AimBot::Active && AimBot::NoSpread) 
	{
		DWORD64 Weapon = (DWORD64)LP->ActiveWeapon();
		if (Weapon) {
			*(float*)(Weapon + Offset::NoSpreadX) = 000000.1;
			*(float*)(Weapon + Offset::NoSpreadY) = 000000.1;
			*(float*)(Weapon + Offset::NoSpreadZ) = 000000.1;
		}
	}*/

	//call original
	SpoofCall(CreateMoveOrg, thisPtr, SeqNum, SampleTime, Active);

	//get usercmd & process entities
	UserCmd* CurCmd = (UserCmd*)(*(DWORD64*)(thisPtr + 0xF8) + 0x218 * (SeqNum % 750));
	ProcessEntityCache(CurCmd);


	Aim(CurCmd);

	/*static uint64_t uFindPos;
	if (!uFindPos) uFindPos = (uint64_t)FindPattern("0F 2E C8 7A 02 74 26");
	auto g_GameSpeedHack = (uFindPos + 7) + *(uint32_t*)(uFindPos + 11) + 8;
	
	static bool flag = false;
	if (CurCmd->buttons & 1) {
		if (!flag) {
			*reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + 0x38) = 0;
			*(float*)g_GameSpeedHack = 4.f; flag = true;
		}

		else
			*(float*)g_GameSpeedHack = 1.f;
	}

	else {
		flag = false;
	}*/
	
	//bhop (w only)
	if (Misc::Bhop && (CurCmd->buttons & 2))
	{
		//forward + duck
		CurCmd->buttons |= 4u;
		CurCmd->forwardmove = 1.f;

		//auto strafer
		if (Global::MouseDelta.x > 0.f)
			CurCmd->sidemove = 1.f;
		else if (Global::MouseDelta.x < 0.f)
			CurCmd->sidemove = -1.f;

		//-jump
		if (!(LP->Flags() & 1)) {
			CurCmd->buttons &= ~2u;
		}
	}


}