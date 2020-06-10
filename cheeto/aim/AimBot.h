#include "BulletPrediction.h"

//Aim Code
void SmoothAngle(Vector3 src, Vector3& dst, float factor) {
	Vector3 delta = dst - src;
	Math::ClampAngle(delta);
	dst = src + delta / factor;
}

CBasePlayer* FindTarget(int& AimEntIndex, int& BestHitBox)
{
	//set global data
	LPlayer* LP_Ent = LP; CBasePlayer* FOV_Target = nullptr;
	float f_Best = AimBot::FOV, d_Best = AimBot::Distance;
	if (!LP_Ent->Alive()) { AimEntIndex = -1; return nullptr; }

	//aim cache
	if (AimEntIndex != -1)
	{
		//check player
		CBasePlayer* CurEnt = Ent(AimEntIndex);
		if (CurEnt->Type(Player))
		{
			//check no knocked & on screen & visible
			auto HBoxPos = CurEnt->HitBoxPos(BestHitBox, false); Vector2 ScreenPos;
			if (!CurEnt->Knocked() && World2Screen(HBoxPos, ScreenPos) && CurEnt->VisiblePos(LP_Ent, HBoxPos)) {
				if (Math::GetFov(LP->DynamicAngles(), Math::CalcAngle(LP->CameraPos(), HBoxPos)) < f_Best)
					return CurEnt;
			}
		}
	}

	//set invalid cache
	AimEntIndex = -1;

	//new find target
	for (int i = 0; i < 80; i++)
	{
		//get entity & check is player
		CBasePlayer* CurEnt = Ent(i);
		if (CurEnt->Type(Player))
		{
			//skip knocked
			if (AimBot::NoKnocked && CurEnt->Knocked())
				continue;

			//cut distance
			Vector3 BodyBonePos = CurEnt->HitBoxPos(3, false);
			float CurDist = Math::GameDist(BodyBonePos, LP->CameraPos());
			if (CurDist > AimBot::Distance) continue;

			//check head
			Vector3 HeadBonePos = CurEnt->HitBoxPos(0, false); Vector2 ScreenPos;
			if (World2Screen(HeadBonePos, ScreenPos) && CurEnt->VisiblePos(LP_Ent, HeadBonePos))
			{
				//calc fov
				float CurFOV = Math::GetFov(LP->DynamicAngles(),
					Math::CalcAngle(LP->CameraPos(), HeadBonePos));

				//set fov target
				if (CurFOV < f_Best) {
					FOV_Target = CurEnt;
					f_Best = CurFOV;
					AimEntIndex = i;
					BestHitBox = 0;
				}
			}

			//check body
			if (AimBot::Nearest && World2Screen(BodyBonePos, ScreenPos) && CurEnt->VisiblePos(LP_Ent, BodyBonePos))
			{
				//calc fov
				float CurFOV = Math::GetFov(LP->DynamicAngles(),
					Math::CalcAngle(LP->CameraPos(), BodyBonePos));

				//set fov target
				if (CurFOV < f_Best) {
					FOV_Target = CurEnt;
					f_Best = CurFOV;
					AimEntIndex = i;
					BestHitBox = 3;
				}
			}
		}
	}
	return FOV_Target;
}

__forceinline bool Aim(UserCmd* Cmd)
{
	static int AimHitBox, AimID = -1;
	
//	if (AimBot::Active && (GetAsyncKeyState(VK_RBUTTON) < 0))
	if(mig::SpoofCall1(GetAsyncKeyState, VK_RBUTTON) < 0)
	{
		float BulletSpeed = LP->ActiveWeapon()->BulletSpeed(); 
		float BulletGrav = LP->ActiveWeapon()->BulletGravity(); 

		if ((BulletSpeed > 1.f) || (BulletGrav > 1.f))
		{		
			Vector3 AimAngles;
			CBasePlayer* AimTarget;
			if (AimTarget = FindTarget(AimID, AimHitBox))
			{
			
				Vector3 MuzzlePos = LP->CameraPos();
				Vector3 TargetPos = AimTarget->HitBoxPos(AimHitBox, true);


				AimAngles = Math::CalcAngle(MuzzlePos, TargetPos);



				Math::ClampAngle(AimAngles);
				Cmd->viewangles = AimAngles;
			}
		}		
	}
	AimID = -1;
	return false;
}