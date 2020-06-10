class UserCmd
{
public:
	int           command_number;
	int           tick_count;
	float         command_time;
	Vector3       viewangles;
	BYTE          _pad0[0x14];
	float         forwardmove;
	float         sidemove;
	float         upmove;
	int           buttons;
	BYTE          impulse;
	BYTE          _pad1[492];
};

struct kbutton_t {
	int down[2];
	int state;
};

class Ray
{
public:
	BYTE RayRaw[0x68];
	__forceinline Ray(const Vector3& Start, const Vector3& End) {
		SpoofCall(Fn::RayInit, RayRaw, &Start, &End);
	}
};

struct surface_t
{
	const char*		name;				//0x0000
	int				surfaceProp;		//0x0008
	unsigned short	flags;				//0x000C
};

struct plane_t
{
	Vector3	surfaceNormal;	//0x0000
	float	dist;			//0x000C
};

struct TraceData
{
	Vector3 startpos;
	int unk0;
	Vector3 endpos;
	int unk01;
	plane_t plane;
	float fraction;
	int contents;
	unsigned short dispFlags;
	bool allsolid;
	bool startsolid;
	int unk02;
	float fractionLeftSolid;
	int unk03;
	surface_t surface;
	int hitgroup;
	short physicsBone;
	unsigned short m_nWorldSurfaceIndex;
	void* m_pEnt;
	int hitbox;
	char pad[0x114];
};

bool World2Screen(const Vector3& Position, Vector2& ScreenPosition)
{
	DWORD64 ViewRender = *(DWORD64*)Offset::ViewRender; 
	if (!ViewRender) return false;

	Matrix4x4* m_Matrix = *(Matrix4x4**)(ViewRender + Offset::ViewMatrix); 
	if (!m_Matrix) return false;

	float w = 0.f;
	ScreenPosition.x = (m_Matrix->m[0][0] * Position.x) + (m_Matrix->m[0][1] * Position.y) + (m_Matrix->m[0][2] * Position.z + m_Matrix->m[0][3]);
	ScreenPosition.y = (m_Matrix->m[1][0] * Position.x) + (m_Matrix->m[1][1] * Position.y) + (m_Matrix->m[1][2] * Position.z + m_Matrix->m[1][3]);
	w = (m_Matrix->m[3][0] * Position.x) + (m_Matrix->m[3][1] * Position.y) + (m_Matrix->m[3][2] * Position.z + m_Matrix->m[3][3]);

	if (w < 0.01f)
		return false;

	float invw = 1.f / w;
	ScreenPosition.x *= invw;
	ScreenPosition.y *= invw;

	float x = Global::WndSize.x / 2.f;
	float y = Global::WndSize.y / 2.f;
	x += .5f * ScreenPosition.x * Global::WndSize.x + .5f;
	y -= .5f * ScreenPosition.y * Global::WndSize.y + .5f;
	ScreenPosition.x = x; ScreenPosition.y = y; return true;
}