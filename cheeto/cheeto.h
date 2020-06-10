#define Ent(Id) (*(CBasePlayer**)(Offset::EntityList + (Id << 5)))
#define Ent1(Id) (*(CBasePlayer**)(Offset::EntityList + (Id << 5)))
#define LP (*(LPlayer**)Offset::LocalPlayer)

enum EntType {
	Player,
	LootEnt,
	NPC
};

class CBasePlayer
{
private:
	int BoneByHitBox(int HitBox)
	{
		DWORD64 Model = *(DWORD64*)(this + Offset::StudioHdr);
		if (!Model) return -1;

		DWORD64 StudioHdr = *(DWORD64*)(Model + 8);
		if (!StudioHdr) return -1;

		DWORD64 HitBoxsArray = StudioHdr + *(int*)(StudioHdr + 0xB4);
		if (!HitBoxsArray) return -1;

		int Bone = *(int*)(HitBoxsArray + *(int*)(HitBoxsArray + 8) + (HitBox * 0x2C));
		return ((Bone < 0) || (Bone > 255)) ? -1 : Bone;
	}

public:
	int Health() {
		if (!this) return 0;
		return *(int*)(this + Offset::Health);
	}

	DWORD Flags() {
		if (!this) return 0;
		return *(DWORD*)(this + Offset::Flags);
	}

	bool Knocked() {
		if (!this) return true;
		return (*(int*)(this + Offset::Bleedout) != 0);
	}
	
	Vector3 AbsOrgin(bool Update = false) {
		if (!this) return Vector3{ 0.f, 0.f, 0.f };
		if (Update) SpoofCall(Fn::UpdateAbsOrigin, this);
		return *(Vector3*)(this + Offset::AbsVecOrgin);
	}

	Vector3 AbsVelocity(bool Update = false) {
		if (!this) return Vector3();
		typedef void(__fastcall* GetAbsFn)(PVOID);
		if (Update) SpoofCall(Fn::UpdateAbsVelocity, this);
		return *(Vector3*)(this + Offset::AbsVelocity);
	}

	int ScriptID() {
		if (!this) return -1;
		return *(int*)(this + Offset::ScriptInt);
	}

	const char* PlayerName() {
		if (!this) return nullptr;
		return SpoofCall<const char*>(Fn::GetEntName, this);
	}

	Vector3 CameraPos() {
		if (!this) return Vector3();
		return *(Vector3*)(this + Offset::CameraPos);
	}

	bool Alive() {
		if (!this) return false;
		return bool(*(int*)(this + Offset::LifeState) == 0);
	}

	bool Type(EntType Type)
	{
		CBasePlayer* LPlayer = *(CBasePlayer**)Offset::LocalPlayer;
		if (!this || !LPlayer || (this == LPlayer)) return false;

		DWORD64 PropHash = *(DWORD64*)(this + Offset::PropName); 
		if (!PropHash) return false; PropHash = *(DWORD64*)PropHash;

		if (Type == LootEnt)
			return (PropHash == 0x7275735F706F7270);
		else if ((PropHash == 0x726579616C70) && Alive()) {
			return (*(int*)(this + Offset::TeamNum) != *(int*)((DWORD64)LPlayer + Offset::TeamNum));
		}
		return false;
	}

	void Glow(const Vector3& Color,const Vector3& Color1)
	{
		*(int*)(this + 0x310) = 1;

		*(bool*)(this + 0x380) = true;

		*(float*)(this + 0x2FC) = FLT_MAX;

		*(Vector3*)(this + 0x1D0) = Color;

		*(float*)(this + 0x2D0) = FLT_MAX;
		*(float*)(this + 0x2D8) = FLT_MAX;
		*(float*)(this + 0x2E0) = FLT_MAX;

		*(float*)(this + 0x2D4) = FLT_MAX;
		*(float*)(this + 0x2DC) = FLT_MAX;
		*(float*)(this + 0x2E4) = FLT_MAX;

		*(float*)(this + 0x2E8) = FLT_MAX;

		*(float*)(this + 0x2EC) = FLT_MAX;
		*(float*)(this + 0x2C4) = FLT_MAX;

		*(Vector3*)(this + 0x1A8) = Color1;
	}

	Vector3 HitBoxPos(int HitBox, bool UpdateBones)
	{
		DWORD64 Bones = *(DWORD64*)(this + Offset::BoneClass); 
		if (!Bones) return Vector3(); Vector3 BoneOff;
		int Bone = BoneByHitBox(HitBox);
		if (Bone == -1) return Vector3();

		if (!UpdateBones) {
			Matrix3x4* BoneMatrix = (Matrix3x4*)(Bones + (Bone * sizeof(Matrix3x4)));
			BoneOff = { BoneMatrix->_14, BoneMatrix->_24, BoneMatrix->_34 };
		}

		else 
		{ 
			static Matrix3x4 MatrixArray[256];
			if (!VCall<bool>((PVOID)(this + 0x10), 16, MatrixArray, 256, 0xFFFF00, 0.f)) 
				return Vector3();

			BoneOff = { MatrixArray[Bone]._14, MatrixArray[Bone]._24, MatrixArray[Bone]._34 };
		}
		
		return AbsOrgin(UpdateBones) + BoneOff;
	}

	
	bool VisiblePos(CBasePlayer* LPlayer, Vector3& Pos) {
		if (!this || !LPlayer) return false;
		Ray RayData(LPlayer->CameraPos(), Pos); TraceData TraceOut;
		VCall(I::EngineTrace, 3, RayData.RayRaw, 0x640400B, nullptr, &TraceOut);
		return (TraceOut.m_pEnt == this || TraceOut.fraction > 0.97f);
	}
};

class Weapon 
{
public:
	float BulletSpeed() {
		if (!this) return 0.f;
		return *(float*)(this + Offset::BulletSpeed);
	}

	float BulletGravity() {
		if (!this) return 0.f;
		return *(float*)(this + Offset::BulletGravity);
	}

	
};

class LPlayer : public CBasePlayer
{
public:
	Weapon* ActiveWeapon() {
		if (!this) return nullptr;
		DWORD64 ActWeaponId = *(DWORD64*)(this + Offset::ActiveWeapon) & 0xFFFF;
		return (ActWeaponId ? (Weapon*)Ent(ActWeaponId) : nullptr);
	}

	Vector3 PunchAngle() {
		if (!this) return Vector3{};
		return *(Vector3*)(this + Offset::PunchAngle);
	}

	__forceinline Vector3 StaticAngles() {
		if (!this) return Vector3{};
		return *(Vector3*)(this + Offset::CameraAng);
	}

	__forceinline void StaticAngles(const Vector3& Ang) {
		if (!this) return;
		*(Vector3*)(this + Offset::CameraAng) = Ang;
	}

	Vector3 DynamicAngles() {
		if (!this) return Vector3{};
		return *(Vector3*)(this + Offset::DynamicAng);
	}
};

class CEntityList
{
public:
	CBasePlayer* Ent;
private:
	int SerialNumber;
	CEntityList* PrevEnt;
	CEntityList* NextEnt;

public:
	__forceinline CEntityList* Start() {
		CEntityList* EntInfo = *(CEntityList**)(Offset::EntityList + 0x200000);
		if (EntInfo) { if (EntInfo->Ent) return EntInfo; else EntInfo->Next(); }
		return nullptr;
	}

	__forceinline CEntityList* Next()
	{
		CEntityList* NextEntry = NextEnt;

		GetValidLoop: 
		if (NextEntry) {
			if (NextEntry->Ent) 
				return NextEntry;

			else {
				NextEntry = NextEntry->NextEnt;
				goto GetValidLoop;
			}
		} 
		
		return nullptr;
	}
};