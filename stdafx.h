#pragma warning(disable : 4530)
#pragma comment(linker,"/MERGE:.rdata=.text")
#pragma comment(linker,"/MERGE:.pdata=.text")
extern "C" int _fltused = 0;

#include <windows.h>
#include <winternl.h>
#include <intrin.h>
#include <stdio.h>
#include <d3d11.h>
#include <cfloat>


#include "cheeto/SpoofCall/EncStr.h"
#include "cheeto/SpoofCall/Utils.h"
#include "cheeto/SpoofCall/Math.h"

namespace AimBot
{
	bool Active = true;
	bool Silent = false; 
	bool Nearest = false;
	bool NoKnocked = true;
	bool PredictType = false;
	bool NoSpread = false;
	float FOV = 150.f;
	float Smooth = 1.f;
	float Distance = 200.f;
}

namespace Visuals
{
	bool NameESP = true;
	bool ESP = true;
	bool HealthESP = true;
	bool LootESP = true;
	bool DrawCrossHair = false;
	bool DrawFOV = true;
	bool SnapLines = true;
	float DistanceESP = 500.f; 
	float DistanceLoot = 100.f;
	bool GlowESP = true;
	bool FOVCircle = true;
}

namespace Misc
{
	bool Bhop = true;
	bool SlowMo = true;
}

namespace Global {
	Vector2 WndSize;
	Vector3 LP_HeadPos;
	Vector2 MouseDelta;
}

namespace Offset
{
	DWORD64 ViewMatrix;
	DWORD64 ViewRender;
	DWORD64 EntityList;
	DWORD64 LocalPlayer;

	DWORD Flags;
	DWORD Health;
	DWORD TeamNum;
	DWORD PropName;
	DWORD Bleedout;
	DWORD LifeState;
	DWORD BoneClass;
	DWORD StudioHdr;
	DWORD ScriptInt;
	DWORD PunchAngle;
	DWORD AbsVelocity;
	DWORD ShieldHealth;
	DWORD ActiveWeapon;
	DWORD CameraPos = 0x1DA4;
	DWORD AbsVecOrgin = 0x14C;
	DWORD BulletSpeed = 0x1d48;
	DWORD BulletGravity = 0x1D50;
	DWORD CameraAng = 0x23C8;
	DWORD DynamicAng = CameraAng - 0x10;
	DWORD NoSpreadX = 0x1D50;
	DWORD NoSpreadY = NoSpreadX - 0x10;
	DWORD NoSpreadZ = NoSpreadY + 0x4;
}

namespace Fn {
	PVOID RayInit;
	PVOID GetEntName;
	PVOID UpdateAbsOrigin;
	PVOID UpdateAbsVelocity;
}

namespace I {
	PVOID CHLClient;
	PVOID EngineTrace; 
	IDXGISwapChain* SwapChain;
}

typedef struct LDR_DATA_TABLE_ENTRY_FIX {
	PVOID Reserved1[2];
	LIST_ENTRY InMemoryOrderLinks;
	PVOID Reserved2[2];
	PVOID DllBase;
	PVOID Reserved3[1];
	ULONG64 SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	PVOID Reserved5[2];
	#pragma warning(push)
	#pragma warning(disable: 4201)
	union {
		ULONG CheckSum;
		PVOID Reserved6;
	} DUMMYUNIONNAME;
	#pragma warning(pop)
	ULONG TimeDateStamp;
};