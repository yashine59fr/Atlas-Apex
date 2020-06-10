#include "stdafx.h"
#include "SDK/NetvarMgr.h"
#include "SDK/GameSDK.h"
#include "cheeto/cheeto.h"
#include "cheeto/CreateMoveW/GameMovement.h"
#include "cheeto/CreateMoveW/CreateMove.h"
#include "cheeto/CreateMoveW/render.h"

BOOL DllMain(HMODULE ModBase, DWORD CallReason, PVOID Reserv)
{
	if (CallReason == DLL_PROCESS_ATTACH)
	{
		FC::SetSpoofStub(FindPattern(E("FF 27")));	
		Fn::UpdateAbsOrigin = (PVOID)RVA(FindPattern(E("E8 ? ? ? ? F3 0F 10 76 ? F3 0F 5C")), 5);
		Fn::RayInit = (PVOID)FindPattern(E("F3 41 0F 10 08 F3 0F 5C 0A F3 0F 11"));
		Fn::GetEntName = (PVOID)FindPattern(E("48 83 3D ? ? ? ? ? 74 08 8B"));
		Fn::UpdateAbsVelocity = (PVOID)FindPattern(E("40 57 48 83 EC 50 F7"));
		Offset::LocalPlayer = RVA(FindPattern(E("48 8D 0D ? ? ? ? 48 8B D7 FF 50 58")), 7) + 8;
		Offset::EntityList = RVA(FindPattern(E("48 8D 05 ? ? ? ? 44 2B")), 7);		
		I::CHLClient = (PVOID)RVA(FindPattern(E("48 8D 0D ? ? ? ? 48 89 0D ? ? ? ? 48 8D 05")), 7);
		I::SwapChain = *(IDXGISwapChain**)RVA(FindPattern(E("4C 8D 0D ? ? ? ? 2B 45")), 7);
		I::EngineTrace = *(PVOID*)(RVA(FindPattern(E("48 8B 0D ? ? ? ? 89 44 24 50")), 7));
		PVOID CInputClass = (PVOID)RVA(FindPattern(E("48 8D 0D ? ? ? ? 44 0F 29 48")), 7);
		PVOID CGameMoveClass = (PVOID)RVA(FindPattern(E("48 8D 0D ? ? ? ? 8B 83")), 7);		
		Offset::PunchAngle = *(DWORD*)(FindPattern(E("E8 ? ? ? ? F2 0F 10 93 ? ? ? ? 0F 28 D8")) + 9);
		Offset::StudioHdr = *(DWORD*)(FindPattern(E("48 83 B9 ? ? ? ? ? 48 8B D9 75 25")) + 3);
		Offset::ViewRender = RVA(FindPattern(xorstr_("48 8B 0D ? ? ? ? 33 D2 48 8B 01 FF 90 ? ? ? ? F3")), 7);
		Offset::AbsVelocity = GetOffset(E("DT_LocalPlayerExclusive"), E("m_vecAbsVelocity"));
		Offset::ViewMatrix = *(DWORD*)(FindPattern(E("48 89 AB ? ? ? ? 4C 89 9B")) + 3);
		Offset::ActiveWeapon = GetOffset(E("DT_Player"), E("m_latestPrimaryWeapons"));
		Offset::BoneClass = GetOffset(E("DT_BaseEntity"), E("m_nForceBone")) + 0x48;
		Offset::ScriptInt = GetOffset(E("DT_PropSurvival"), E("m_customScriptInt"));
		Offset::PropName = GetOffset(E("DT_BaseEntity"), E("m_iSignifierName"));
		Offset::Bleedout = GetOffset(E("DT_Player"), E("m_bleedoutState"));
		Offset::LifeState = GetOffset(E("DT_Player"), E("m_lifeState"));
		Offset::TeamNum = GetOffset(E("DT_Player"), E("m_iTeamNum"));
		Offset::Health = GetOffset(E("DT_Player"), E("m_iHealth"));
		Offset::Flags = GetOffset(E("DT_Player"), E("m_fFlags"));		
		VMT_Hook CGameMove(nullptr, CGameMoveClass);
		CGameMove.SetupHook(&SilentEndOrg, SilentEnd, 5);
		CGameMove.SetupHook(&SilentStartOrg, SilentStart, 4);
		VMT_Hook CCreateMove(nullptr, CInputClass);
		CCreateMove.SetupHook(&CreateMoveOrg, CreateMoveHk, 4);		
		BYTE ClassOffset = *(BYTE*)(FindPattern(E("8B ? ? ? ? ? 48 8B 01 48 8D 55"), E("dxgi")) + 2);
		VMT_Hook D3D(E("d3d11"), *(DWORD64*)((DWORD64)I::SwapChain + ClassOffset));
		D3D.SetupHook(&hk_presentOrg, hk_present, 13);
	} 
	return true;
}