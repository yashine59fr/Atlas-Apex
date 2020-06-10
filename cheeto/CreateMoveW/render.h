#include "../d3d/D3D_Experimental/D3D_Render.h"
#include "../d3d/Gui.h"

Vector2 MenuPos = { 200.f, 200.f };

__forceinline const wchar_t* LootName(int ScriptID, Render::Color& Color)
{
	switch (ScriptID)
	{
		//energy
	case 6:  Color = Render::Color(255, 255, 0); return xorstr_(L"Havoc");
	case 4:  Color = Render::Color(255, 255, 0); return xorstr_(L"L-Star");
	case 45: Color = Render::Color(255, 255, 0); return xorstr_(L"ENERGY");

		//gold
	case 1:  Color = Render::Color(255, 236, 66); return xorstr_(L"Kraber");
	case 33:  Color = Render::Color(255, 236, 66); return xorstr_(L"PeaceKeeper");
	case 8:  Color = Render::Color(255, 236, 66); return xorstr_(L"Devotion");

		//sniper
	case 9:  Color = Render::Color(96, 102, 208); return xorstr_(L"Triple-Take");
	case 42: Color = Render::Color(96, 102, 208); return xorstr_(L"Sentinel");
	case 23: Color = Render::Color(96, 102, 208); return xorstr_(L"LongBow");
	case 25: Color = Render::Color(96, 102, 208); return xorstr_(L"Volt");
	case 48: Color = Render::Color(96, 102, 208); return xorstr_(L"SNIPER");

		//heavy
	case 11:  Color = Render::Color(0, 191, 255); return xorstr_(L"FlatLine");
	case 13: Color = Render::Color(0, 191, 255); return xorstr_(L"Hemlok");
	case 21: Color = Render::Color(0, 191, 255); return xorstr_(L"Prowler");
	case 27: Color = Render::Color(0, 191, 255); return xorstr_(L"SpitFire");
	case 36: Color = Render::Color(0, 191, 255); return xorstr_(L"WingMan");
	case 47: Color = Render::Color(0, 191, 255); return xorstr_(L"HEAVY");

		//light
	case 15: Color = Render::Color(255, 128, 0); return xorstr_(L"G7-Scout");
	case 17: Color = Render::Color(255, 128, 0); return xorstr_(L"Alternator");
	case 19: Color = Render::Color(255, 128, 0); return xorstr_(L"R-99");
	case 29: Color = Render::Color(255, 128, 0); return xorstr_(L"R-301");
	case 38: Color = Render::Color(255, 128, 0); return xorstr_(L"P2020");
	case 40: Color = Render::Color(255, 128, 0); return xorstr_(L"RE-45");
	case 44: Color = Render::Color(255, 128, 0); return xorstr_(L"LIGHT");

		//shotgun
	case 31: Color = Render::Color(255, 0, 0); return xorstr_(L"EVA-8");
	case 2: Color = Render::Color(255, 0, 0); return xorstr_(L"Mastiff");
	case 34: Color = Render::Color(255, 0, 0); return xorstr_(L"Mozambique");
	case 46: Color = Render::Color(255, 0, 0); return xorstr_(L"FRACTION");

		//misc
	case 49: Color = Render::Color(0, 191, 255); return xorstr_(L"Accelerant");
	case 50: Color = Render::Color(0.65f, 0.27f, 1.f, 1.f); return xorstr_(L"Phoenix");
	case 51: Color = Render::Color(255, 10, 0); return xorstr_(L"MedKit");
	case 52: Color = Render::Color(255, 10, 0); return xorstr_(L"Syringe");
	case 53: Color = Render::Color(0, 191, 255); return xorstr_(L"SH-Battery");
	case 54: Color = Render::Color(0, 191, 255); return xorstr_(L"SH-Cell");

		//helmet
	case 55: Color = Render::Color(255, 255, 255); return xorstr_(L"Helmet-1");
	case 56: Color = Render::Color(0, 191, 255); return xorstr_(L"Helmet-2");
	case 57: Color = Render::Color(0.65f, 0.27f, 1.f, 1.f); return xorstr_(L"Helmet-3");
	case 58: Color = Render::Color(255, 215, 0); return xorstr_(L"Helmet-4");

		//armor
	case 59: Color = Render::Color(255, 255, 255); return xorstr_(L"Armor-1");
	case 60: Color = Render::Color(0, 191, 255); return xorstr_(L"Armor-2");
	case 61: Color = Render::Color(0.65f, 0.27f, 1.f, 1.f); return xorstr_(L"Armor-3");
	case 62: Color = Render::Color(255, 215, 0); return xorstr_(L"Armor-4");
		default: return nullptr;
	}
}

PVOID hk_presentOrg = nullptr;

void __fastcall hk_present(PVOID a1, PVOID a2)
{
	if (Render::NewFrame(I::SwapChain, Global::WndSize))
	{
		if (Visuals::FOVCircle)
			Render::Circle(Global::WndSize / 2.f, Render::Color(255, 255, 255), AimBot::FOV, 2.f);

		for (int i = 0; i < EntityCount; i++)
		{
			if (EntityCache[i].EntID == -1) continue;
			CBasePlayer* CurEnt = Ent(EntityCache[i].EntID);
			
			if (Visuals::LootESP && CurEnt->Type(LootEnt))
			{
				Vector2 LootPos2D; Render::Color Color;
				if (World2Screen(CurEnt->AbsOrgin(), LootPos2D))
				{
					const wchar_t* LootStr = LootName(CurEnt->ScriptID(), Color);
					Vector2 LootTextSize = Render::String(LootPos2D + 7, LootStr, true, Color);

				}
			}

			 if (Visuals::ESP && CurEnt->Type(Player))
			{
				Vector2 Head2D, Feet2D; Vector3 Head3D = CurEnt->HitBoxPos(0, false);
				if (World2Screen(Head3D, Head2D) && World2Screen(CurEnt->AbsOrgin(), Feet2D))
				{
					float Height = (Head2D.y - Feet2D.y), Width = Height / 2.f;

					Render::Color BoxColor = Render::Color(255, 0, 0);
					if (CurEnt->Knocked()) BoxColor = Render::Color(192, 192, 192);
					else if (EntityCache[i].Visible) BoxColor = Render::Color(0, 255, 0);

					Render::Rectangle({ (Feet2D.x - (Width / 2.f) + 4.f), Feet2D.y }, { Width, Height }, BoxColor, 0.5f);

					Render::FillRectangle({ (Feet2D.x - (Width / 2.f) + 4.f), Feet2D.y }, { Width , Height }, { 0.f, 0.f, 0.f, 0.75f });

					if (Visuals::GlowESP) 
					{
						Vector3 Color1 = { 130.f, 0.f, 1.f };

						Vector3 Color = { 130.f, 0.f, 1.f };
						if (CurEnt->Knocked()) Color = { 130.f, 130.f, 130.f };
						else if (EntityCache[i].Visible) Color = { 0.f, 130.f, 1.f };
						CurEnt->Glow(Color, Color1);
					}

					if (Visuals::HealthESP)
					{
						float HealthSize = (Height * CurEnt->Health()) / 100.f;
						Render::Line({ (Feet2D.x - (Width / 2.f) + 4.f), Feet2D.y + HealthSize }, { (Feet2D.x - (Width / 2.f) + 4.f), Head2D.y }, Render::Color(255, 0, 0), 2.f);
						Render::Line({ (Feet2D.x - (Width / 2.f) + 4.f), Feet2D.y }, { (Feet2D.x - (Width / 2.f) + 4.f), Feet2D.y + HealthSize }, Render::Color(0, 255, 0), 2.f);
					}
					
					if (Visuals::SnapLines)
					{
						Render::Line({Global::WndSize.x / 2.f, Global::WndSize.y / 40.f }, { (Feet2D.x - (Width / 2.f) + 4.f), Feet2D.y }, BoxColor, 2.f);
					}

					if (Visuals::DrawCrossHair)
					{
						Render::Line({ Global::WndSize / 2.f }, { Global::WndSize / 2.f }, Render::Color(255, 255, 255), 2.f);
					}

					if (Visuals::NameESP)
					{
						const char* Name = CurEnt->PlayerName();
						if (!Name) continue; wchar_t Str[64];
						for (int i = 0;; i++) { if (!(Str[i] = Name[i])) break; }
						Render::String({ Feet2D.x, Head2D.y - 12.f }, Str, true, Render::Color(255,255,255));
					}
						
				}
			}
		}
		
		
		if (GUI::Visible) 
		{			
			GUI::Begin(E(L"Apex"), MenuPos, { 300.f, 500.f }, { Render::Color(255.f, 0.f, 0.f, 0.80f)});
			GUI::Spacing(E(L"Visuals"));
			GUI::CheckBox(E(L"enemy-esp"), Visuals::ESP);
			GUI::CheckBox(E(L"health-esp"), Visuals::HealthESP);
			GUI::CheckBox(E(L"snap-lines"), Visuals::SnapLines);
			GUI::CheckBox(E(L"glow-esp"), Visuals::GlowESP);
			GUI::CheckBox(E(L"name-esp"), Visuals::NameESP);
			GUI::Spacing(E(L"Aimbot"));
			GUI::CheckBox(E(L"draw-fov"), Visuals::FOVCircle);
			GUI::CheckBox(E(L"memory-aim"), AimBot::Active);
		 	GUI::SliderFloat(E(L"aim-smooth"), &AimBot::Smooth, 1.f, 10.f);
		//	GUI::CheckBox(E(L"silent-aim (buggy)"), AimBot::Silent);
			GUI::SliderFloat(E(L"aim-fov"), &AimBot::FOV, 10.f, 200.f);
			GUI::Spacing(E(L"Extra"));
			GUI::CheckBox(E(L"loot-esp"), Visuals::LootESP);
			GUI::CheckBox(E(L"bunny-hop"), Misc::Bhop);		
			GUI::End();
		}

		Render::EndFrame(I::SwapChain);
	}

	static bool init;
	if (!init) {
		
		auto ThID = FC(kernel32, GetCurrentThreadId);
		FC(user32, SetWindowsHookExW, WH_GETMESSAGE, GUI::WndHook, (HINSTANCE)GetModuleBase(nullptr), ThID);
		init = true;
	}

	ret:
	SpoofCall(hk_presentOrg, a1, a2); 
}

