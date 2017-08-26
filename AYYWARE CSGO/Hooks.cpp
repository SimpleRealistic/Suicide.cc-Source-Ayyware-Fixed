#include "Hooks.h"
#include "Hacks.h"
#include "Chams.h"
#include "Menu.h"

#include "Interfaces.h"
#include "RenderManager.h"
#include "MiscHacks.h"
#include "CRC32.h"
#include "Resolver.h"
#include "IPrediction.h"
#include "ResolverMeme.h"

#include <math.h>
#include <cmath>
#include <ctime>
#include <intrin.h>
#include <stdio.h>
#include <time.h>
#include "igameeventmanager.h"
#include <Windows.h>
#include "Sounds.h"
#include "playsoundapi.h"

#pragma comment(lib, "Winmm.lib")
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin)
Vector LastAngleAA;
Vector LastAngleAAFake;
#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094330572703657595919530921861173819326117931051185480744623799627495673518857527248912279381830119491298336733624406566430860213949463952247371907021798609437027705392171762931767523846748184676694051320005681271452635608277857713427577896091736371787214684409012249534301465495853710507922796892589235420199561121290219608640344181598136297747713099605187072113499999983729780499510597317328160963185950244594553469083026425223082533446850352619311881710100031378387528865875332083814206171776691473035982534904287554687311595628638823537875937519577818577805321712268066130019278766111959092164201989380952572010654858632788659361533818279682303019520353018529689957736225994138912497217752834791315155748572424541506959508295331168617278558890750983817546374649393192550604009277016711390098488240128583616035637076601047101819429555961989467678374494482553797747268471040475346462080466842590694912933136770289891521047521620569660240580
inline float DegreesToRadians(float Angle) { return Angle * M_PI / 180.0f; }

// Funtion Typedefs
typedef void(__thiscall* DrawModelEx_)(void*, void*, void*, const ModelRenderInfo_t&, matrix3x4*);
typedef void(__thiscall* PaintTraverse_)(PVOID, unsigned int, bool, bool);
typedef bool(__thiscall* InPrediction_)(PVOID);
typedef void(__stdcall *FrameStageNotifyFn)(ClientFrameStage_t);
typedef void(__thiscall* RenderViewFn)(void*, CViewSetup&, CViewSetup&, int, int);

using OverrideViewFn = void(__fastcall*)(void*, void*, CViewSetup*);
typedef float(__stdcall *oGetViewModelFOV)();

// Function Pointers to the originals
PaintTraverse_ oPaintTraverse;
DrawModelEx_ oDrawModelExecute;
FrameStageNotifyFn oFrameStageNotify;
OverrideViewFn oOverrideView;
RenderViewFn oRenderView;

// Hook function prototypes
void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
bool __stdcall Hooked_InPrediction();
void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld);
bool __stdcall CreateMoveClient_Hooked(/*void* self, int edx,*/ float frametime, CUserCmd* pCmd);
void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage);
void __fastcall Hooked_OverrideView(void* ecx, void* edx, CViewSetup* pSetup);
float __stdcall GGetViewModelFOV();
void __fastcall Hooked_RenderView(void* ecx, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);


class PlayerList
{
public:

	IClientEntity* pLocalEz = hackManager.pLocal();
	class CPlayer
	{
	public:
		Vector ShootPos[125];
		int index = -1;
		IClientEntity* entity;
		Vector reset = Vector(0, 0, 0);
		float lastsim = 0;
		Vector lastorigin = Vector(0, 0, 0);
		std::vector< float > pastangles;
		int ScannedNumber = 0;
		int BestIndex = 0;
		float difference = 0.f;
		float Backtrack[360];
		float flLastPelvisAng = 0.f;
		float flEyeAng = 0.f;
		float resolved = 0.f;
		float posedifference = 0.f;
		Hitbox* box;

		CPlayer(IClientEntity* entity, int index, int lastsim) : entity(entity), index(index), lastsim(lastsim)
		{
		}
	};

private:
	std::vector< CPlayer > Players;
public:
	void Update()
	{
		for (int i = 0; i < Players.size(); i++)
		{
			if (Players[i].entity == nullptr)
			{
				Players.erase(Players.begin() + i);
				continue;
			}
			if (Players[i].entity == pLocalEz)
			{
				Players.erase(Players.begin() + i);
				continue;
			};
		}
	}

	void UpdateSim()
	{
		Update();
		for (int i = 0; i < Players.size(); i++)
		{
			Players[i].lastsim = Players[i].entity->GetSimulationTime();
		}
	}

	void AddPlayer(IClientEntity* ent)
	{
		Players.emplace_back(CPlayer(ent, ent->GetIndex(), ent->GetSimulationTime()));
	}

	CPlayer* FindPlayer(IClientEntity* ent)
	{
		for (int i = 0; i < Players.size(); i++)
			if (Players[i].index == ent->GetIndex())
				return &Players[i];
		AddPlayer(ent);
		return FindPlayer(ent);
	}
} plist;

// VMT Managers
namespace Hooks
{
	// VMT Managers
	Utilities::Memory::VMTManager VMTPanel; // Hooking drawing functions
	Utilities::Memory::VMTManager VMTClient; // Maybe CreateMove
	Utilities::Memory::VMTManager VMTClientMode; // CreateMove for functionality
	Utilities::Memory::VMTManager VMTModelRender; // DrawModelEx for chams
	Utilities::Memory::VMTManager VMTPrediction; // InPrediction for no vis recoil
	Utilities::Memory::VMTManager VMTPlaySound; // Autoaccept 
	Utilities::Memory::VMTManager VMTRenderView;
};
DamageEventListener* DamageListener;
DamageEventListener* KillListener;
DamageEventListener* roundstart;
DamageEventListener* round_end;
// Undo our hooks
void Hooks::UndoHooks()
{
	VMTPanel.RestoreOriginal();
	VMTPrediction.RestoreOriginal();
	VMTModelRender.RestoreOriginal();
	VMTClientMode.RestoreOriginal();
	Interfaces::GameEventManager->RemoveListener(DamageListener);
	delete DamageListener;
}

int hitmarkertime = 0;
void imfinnarunuponya(IGameEvent* pEvent)
{
	if (Menu::Window.MiscTab.OtherHitmarker.GetIndex() > 0)
	{

		int attackerid = pEvent->GetInt("attacker");
		int entityid = Interfaces::Engine->GetPlayerForUserID(attackerid);
		if (entityid == Interfaces::Engine->GetLocalPlayer())
		{
			hitmarkertime = 255;
			switch (Menu::Window.MiscTab.OtherHitmarker.GetIndex())
			{
			case 0:
				break;
			case 1:
				PlaySoundA(rawData, NULL, SND_ASYNC | SND_MEMORY);
				break;
			case 2:
				PlaySoundA(pew, NULL, SND_ASYNC | SND_MEMORY);
				break;
			case 3:
				PlaySoundA(pewsilent, NULL, SND_ASYNC | SND_MEMORY);
				break;
			case 4:
				PlaySoundA(roblox, NULL, SND_ASYNC | SND_MEMORY);
				break;
			}
		}
	}

}
player_info_t GetInfo(int Index) {
	player_info_t Info;
	Interfaces::Engine->GetPlayerInfo(Index, &Info);
	return Info;
}
void imfinnamemeu(IGameEvent* pEvent)
{

	// char *
	int attackerid = pEvent->GetInt("attacker");
	int entityid = Interfaces::Engine->GetPlayerForUserID(attackerid);

	if (entityid == Interfaces::Engine->GetLocalPlayer())
	{
		int nUserID = pEvent->GetInt("attacker");
		int nDead = pEvent->GetInt("userid");
		if (nUserID || nDead)
		{
			bool headshot = pEvent->GetBool("headshot");
			if (Menu::Window.MiscTab.OtherKillSpam.GetState())
			{
				if (headshot)
				{

					PlaySoundA(headmeme, NULL, SND_ASYNC | SND_MEMORY);

				}
			}
			else
				if (Menu::Window.MiscTab.OtherKillSpamchat.GetState())
				{
					if (headshot)
					{

						player_info_t killed_info = GetInfo(Interfaces::Engine->GetPlayerForUserID(nDead));
						std::string parttwo = killed_info.name;
						std::string partthree = (" was 1taped by SUICIDE.CC. You sell?");
						SayInChat(("Player [" + parttwo + "]" + partthree + "[SUICIDE.CC]").c_str());

					}
					else
					{
						player_info_t killed_info = GetInfo(Interfaces::Engine->GetPlayerForUserID(nDead));
						std::string parttwo = killed_info.name;
						std::string partthree = (" was hitscaned by SUICIDE.CC. Dont cry 'BAIM' Noob");
						SayInChat(("Player [" + parttwo + "]" + partthree + "[SUICIDE.CC]").c_str());
					}
				}

		}
	}

}
// Initialise all our hooks
void Hooks::Initialise()
{
	// Panel hooks for drawing to the screen via surface functions
	VMTPanel.Initialise((DWORD*)Interfaces::Panels);
	oPaintTraverse = (PaintTraverse_)VMTPanel.HookMethod((DWORD)&PaintTraverse_Hooked, Offsets::VMT::Panel_PaintTraverse);
	//Utilities::Log("Paint Traverse Hooked");

	// No Visual Recoi	l
	VMTPrediction.Initialise((DWORD*)Interfaces::Prediction);
	VMTPrediction.HookMethod((DWORD)&Hooked_InPrediction, 14);
	//Utilities::Log("InPrediction Hooked");

	// Chams
	VMTModelRender.Initialise((DWORD*)Interfaces::ModelRender);
	oDrawModelExecute = (DrawModelEx_)VMTModelRender.HookMethod((DWORD)&Hooked_DrawModelExecute, Offsets::VMT::ModelRender_DrawModelExecute);
	//Utilities::Log("DrawModelExecute Hooked");

	//DamageListener = new DamageEventListener(imfinnarunuponya);
	// ...In injection thread
//	Interfaces::GameEventManager->AddListener(DamageListener, "player_hurt", false);
	DamageListener = new DamageEventListener(imfinnarunuponya);
	// ...In injection thread
	Interfaces::GameEventManager->AddListener(DamageListener, "player_hurt", false);
	// Setup ClientMode Hooks
	VMTClientMode.Initialise((DWORD*)Interfaces::ClientMode);
	VMTClientMode.HookMethod((DWORD)CreateMoveClient_Hooked, 24);


	round_end = new DamageEventListener(imfinnamemeu);
	// ...In injection thread
	Interfaces::GameEventManager->AddListener(round_end, "player_death", false);

	oOverrideView = (OverrideViewFn)VMTClientMode.HookMethod((DWORD)&Hooked_OverrideView, 18);
	VMTClientMode.HookMethod((DWORD)&GGetViewModelFOV, 35);

	// Setup client hooks
	VMTClient.Initialise((DWORD*)Interfaces::Client);
	oFrameStageNotify = (FrameStageNotifyFn)VMTClient.HookMethod((DWORD)&Hooked_FrameStageNotify, 36);

}

void MovementCorrection(CUserCmd* pCmd)
{

}

//---------------------------------------------------------------------------------------------------------
//                                         Hooked Functions
//---------------------------------------------------------------------------------------------------------

// Animated ClanTag Function
void SetClanTag(const char* tag, const char* name)
{
	static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(((DWORD)Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30", "xxxxxxxxx????xxxxxx")));
	pSetClanTag(tag, name);
}

// Blank Clantag
void NoClantag()
{
	SetClanTag("", "");
}

// Clantag Functions
void ClanTag()
{
	int speed = Menu::Window.MiscTab.OtherClantagspeed.GetValue();
	static int counter = 0;
	switch (Menu::Window.MiscTab.OtherClantag.GetIndex())
	{
	case 0:
		// No 
		break;
	case 1:
	{
		static int motion = 0;
		int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * speed;

		if (counter % 48 == 0)
			motion++;
		int value = ServerTime % 20;
		switch (value) {
		case 0:SetClanTag("SUICIDE.ME", "SUICIDE.CC"); break;
		case 1:SetClanTag("         S", "SUICIDE.CC"); break;
		case 2:SetClanTag("        SU", "SUICIDE.CC"); break;
		case 3:SetClanTag("       SUI", "SUICIDE.CC"); break;
		case 4:SetClanTag("      SUIC", "SUICIDE.CC"); break;
		case 5:SetClanTag("     SUICI", "SUICIDE.CC"); break;
		case 6:SetClanTag("    SUICID", "SUICIDE.CC"); break;
		case 7:SetClanTag("   SUICIDE", "SUICIDE.CC"); break;
		case 8:SetClanTag("  SUICIDE.", "SUICIDE.CC"); break;
		case 9:SetClanTag(" SUICIDE.C", "SUICIDE.CC"); break;
		case 10:SetClanTag("SUICIDE.CC", "SUICIDE.CC"); break;
		case 11:SetClanTag("UICIDE.CC ", "SUICIDE.CC"); break;
		case 12:SetClanTag("ICIDE.CC   ", "SUICIDE.CC"); break;
		case 13:SetClanTag("CIDE.CC    ", "SUICIDE.CC"); break;
		case 14:SetClanTag("IDE.CC     ", "SUICIDE.CC"); break;
		case 15:SetClanTag("DE.CC      ", "SUICIDE.CC"); break;
		case 16:SetClanTag("E.CC       ", "SUICIDE.CC"); break;
		case 17:SetClanTag(".CC        ", "SUICIDE.CC"); break;
		case 18:SetClanTag("CC         ", "SUICIDE.CC"); break;
		case 19:SetClanTag("c          ", "SUICIDE.CC"); break;
		}
		counter++;
	}
	break;
	
	break;
	case 2:
		// stainless
		SetClanTag("\r", "\r");
		break;
	case 3:
		SetClanTag("[VALV\xE1\xB4\xB1]", "Valve");
		break;
	case 4:
	{
		static int motion = 0;
		int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * speed;

		if (counter % 48 == 0)
			motion++;
		int value = ServerTime % 20;
		switch (value) {
		case 0:SetClanTag("S/", "SUICIDE.CC"); break;
		case 1:SetClanTag("S\\", "SUICIDE.CC"); break;
		case 2:SetClanTag("S_", "SUICIDE.CC"); break;
		case 3:SetClanTag("SU", "SUICIDE.CC"); break;
		case 4:SetClanTag("SU/", "SUICIDE.CC"); break;
		case 5:SetClanTag("SU\\", "SUICIDE.CC"); break;
		case 6:SetClanTag("SU_", "SUICIDE.CC"); break;
		case 7:SetClanTag("SUI", "SUICIDE.CC"); break;
		case 8:SetClanTag("SUI/", "SUICIDE.CC"); break;
		case 9:SetClanTag("SUI\\", "SUICIDE.CC"); break;
		case 10:SetClanTag("SUI_", "SUICIDE.CC"); break;
		case 11:SetClanTag("SUIC", "SUICIDE.CC"); break;
		case 12:SetClanTag("SUIC/", "SUICIDE.CC"); break;
		case 13:SetClanTag("SUIC\\", "SUICIDE.CC"); break;
		case 14:SetClanTag("SUIC_", "SUICIDE.CC"); break;
		case 15:SetClanTag("SUICI", "SUICIDE.CC"); break;
		case 16:SetClanTag("SUICI/", "SUICIDE.CC"); break;
		case 17:SetClanTag("SUICI\\", "SUICIDE.CC"); break;
		case 18:SetClanTag("SUICID", "SUICIDE.CC"); break;
		case 19:SetClanTag("SUICID/", "SUICIDE.CC"); break;
		case 20:SetClanTag("SUICID\\", "SUICIDE.CC"); break;
		case 21:SetClanTag("SUICID_", "SUICIDE.CC"); break;
		case 22:SetClanTag("SUICIDE/", "SUICIDE.CC"); break;
		case 23:SetClanTag("SUICIDE\\", "SUICIDE.CC"); break;
		case 24:SetClanTag("SUICIDE_", "SUICIDE.CC"); break;
		case 25:SetClanTag("SUICIDE.", "SUICIDE.CC"); break;
		case 26:SetClanTag("SUICIDE./", "SUICIDE.CC"); break;
		case 27:SetClanTag("SUICIDE.\\", "SUICIDE.CC"); break;
		case 28:SetClanTag("SUICIDE._", "SUICIDE.CC"); break;
		case 29:SetClanTag("SUICIDE.C", "SUICIDE.CC"); break;
		case 30:SetClanTag("SUICIDE.C/", "SUICIDE.CC"); break;
		case 31:SetClanTag("SUICIDE.C\\", "SUICIDE.CC"); break;
		case 32:SetClanTag("SUICIDE.C_", "SUICIDE.CC"); break;
		case 33:SetClanTag("SUICIDE.CC", "SUICIDE.CC"); break;
		case 34:SetClanTag("SUICIDE.C", "SUICIDE.CC"); break;
		case 35:SetClanTag("SUICIDE.", "SUICIDE.CC"); break;
		case 36:SetClanTag("SUICIDE", "SUICIDE.CC"); break;
		case 37:SetClanTag("SUICID", "SUICIDE.CC"); break;
		case 38:SetClanTag("SUICI", "SUICIDE.CC"); break;
		case 39:SetClanTag("SUIC", "SUICIDE.CC"); break;
		case 40:SetClanTag("SUI", "SUICIDE.CC"); break;
		case 41:SetClanTag("SU", "SUICIDE.CC"); break;
		case 42:SetClanTag("S", "SUICIDE.CC"); break;
		case 43:SetClanTag("R.I.P ME SUICIDE", "SUICIDE.CC"); break;

		}
		counter++;
	}
	break;
	case 5:
		time_t now = time(0);
		char timestamp[10] = "";
		strftime(timestamp, 10, "%H:%M:%S", localtime(&now));
		SetClanTag(timestamp, "Time");
		break;
	
	}
}

// Rank Revealer
void MsgFunc_ServerRankRevealAll()
{
	using MsgFunc_ServerRankRevealAllFn = bool(__cdecl*)(float*);
	static MsgFunc_ServerRankRevealAllFn MsgFunc_ServerRankRevealAll = reinterpret_cast<MsgFunc_ServerRankRevealAllFn>((PDWORD)Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00", "xxxxx????x????"));

	float fArray[3];
	fArray[0] = 0.f;
	fArray[1] = 0.f;
	fArray[2] = 0.f;

	MsgFunc_ServerRankRevealAll(fArray);
}

BYTE bMoveData[0x200];

// Movement Prediction
void Prediction(CUserCmd* pCmd, IClientEntity* LocalPlayer)
{
	if (Interfaces::MoveHelper && Menu::Window.RageBotTab.AimbotEnable.GetState() && Menu::Window.RageBotTab.AccuracyPrediction.GetState() && LocalPlayer->IsAlive())
	{
		float curtime = Interfaces::Globals->curtime;
		float frametime = Interfaces::Globals->frametime;
		int iFlags = LocalPlayer->GetFlags();

		Interfaces::Globals->curtime = (float)LocalPlayer->GetTickBase() * Interfaces::Globals->interval_per_tick;
		Interfaces::Globals->frametime = Interfaces::Globals->interval_per_tick;

		Interfaces::MoveHelper->SetHost(LocalPlayer);

		Interfaces::GamePrediction->SetupMove(LocalPlayer, pCmd, nullptr, bMoveData);
		Interfaces::GameMovement->ProcessMovement(LocalPlayer, bMoveData);
		Interfaces::GamePrediction->FinishMove(LocalPlayer, pCmd, bMoveData);

		Interfaces::MoveHelper->SetHost(0);

		Interfaces::Globals->curtime = curtime;
		Interfaces::Globals->frametime = frametime;
		*LocalPlayer->GetPointerFlags() = iFlags;
	}
}
/*void __stdcall Hooked_PlaySound(const char* pSample)
{
	Hacks.oPlaySound(pSample);

	if (strstr(pSample, "weapons/hegrenade/beep.wav"))
	{
		if (!Settings.GetMenuSetting(Tab_Misc, Misc_AutoAccept))
			return;

		Interfaces::Engine->ClientCmd_Unrestricted("clear");
		Interfaces::Engine->ClientCmd_Unrestricted("echo SOUND_FILE_FOUND");
		DWORD dwIsReady = Utilities::Memory::FindPatternV2("client.dll", "55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ? ? ? ? 57 8B 8E");
		reinterpret_cast< void(*)() >(dwIsReady)();
	}
}*/
// Create moves
bool __stdcall CreateMoveClient_Hooked(float frametime, CUserCmd* pCmd)
{

	if (!pCmd->command_number)
		return true;

	// Create Move for Rank Revealer
	if (Interfaces::Engine->IsInGame() && Interfaces::Engine->IsConnected() && Interfaces::Globals->curtime) {
		if (pCmd->buttons & IN_SCORE && Menu::Window.MiscTab.RankRevealer.GetState()) {
			MsgFunc_ServerRankRevealAll();
		}
	}
	IClientEntity* pLocalEz228 = hackManager.pLocal();
	
	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocalEz228)
	{
		plist.Update();
		PVOID pebp;
		__asm mov pebp, ebp;
		bool* pbSendPacket = (bool*)(*(DWORD*)pebp - 0x1C);
		bool& bSendPacket = *pbSendPacket;
		CClientState* pClient;
		INetChannel* pNet;
		CInput* pInput;

		if (Menu::Window.MiscTab.OtherClantag.GetIndex() > 0)
			ClanTag();

		//	CUserCmd* cmdlist = *(CUserCmd**)((DWORD)Interfaces::pInput + 0xEC);
		//	CUserCmd* pCmd = &cmdlist[sequence_number % 150];


		// Backup for safety
		Vector origView = pCmd->viewangles;
		Vector viewforward, viewright, viewup, aimforward, aimright, aimup;
		Vector qAimAngles;
		qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
		AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);

		// Do da hacks
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal && pLocal->IsAlive())
			Hacks::MoveHacks(pCmd, bSendPacket);

		// Movement Fix
		qAimAngles.Init(0.0f, GetAutostrafeView().y, 0.0f);
		AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);
		qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
		AngleVectors(qAimAngles, &aimforward, &aimright, &aimup);
		Vector vForwardNorm;		Normalize(viewforward, vForwardNorm);
		Vector vRightNorm;			Normalize(viewright, vRightNorm);
		Vector vUpNorm;				Normalize(viewup, vUpNorm);

		// Movement Prediction
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal)
		{
			if (pLocal->IsAlive())
			{
				Prediction(pCmd, pLocal);
			}
		}

		// Original shit for movement correction
		float forward = pCmd->forwardmove;
		float right = pCmd->sidemove;
		float up = pCmd->upmove;
		if (forward > 450) forward = 450;
		if (right > 450) right = 450;
		if (up > 450) up = 450;
		if (forward < -450) forward = -450;
		if (right < -450) right = -450;
		if (up < -450) up = -450;
		pCmd->forwardmove = DotProduct(forward * vForwardNorm, aimforward) + DotProduct(right * vRightNorm, aimforward) + DotProduct(up * vUpNorm, aimforward);
		pCmd->sidemove = DotProduct(forward * vForwardNorm, aimright) + DotProduct(right * vRightNorm, aimright) + DotProduct(up * vUpNorm, aimright);
		pCmd->upmove = DotProduct(forward * vForwardNorm, aimup) + DotProduct(right * vRightNorm, aimup) + DotProduct(up * vUpNorm, aimup);

		// Angle normalisation
		if (Menu::Window.MiscTab.OtherSafeMode.GetState())
		{
			GameUtils::NormaliseViewAngle(pCmd->viewangles);

			if (pCmd->viewangles.z != 0.0f)
			{
				pCmd->viewangles.z = 0.00;
			}

			if (pCmd->viewangles.x < -89 || pCmd->viewangles.x > 89 || pCmd->viewangles.y < -180 || pCmd->viewangles.y > 180)
			{
				GameUtils::NormaliseViewAngle(pCmd->viewangles);
				Beep(750, 800);
				if (pCmd->viewangles.x < -89 || pCmd->viewangles.x > 89 || pCmd->viewangles.y < -180 || pCmd->viewangles.y > 180)
				{
					pCmd->viewangles = origView;
					pCmd->sidemove = right;
					pCmd->forwardmove = forward;
				}
			}
		}

		if (pCmd->viewangles.x > 90)
		{
			pCmd->forwardmove = -pCmd->forwardmove;
		}
	
		if (pCmd->viewangles.x < -90)
		{
			pCmd->forwardmove = -pCmd->forwardmove;
		}
		if (Menu::Window.VisualsTab.OtherWorld.GetIndex() == 0)
			{

				static bool memes;
				if (memes = false)
				{

					for (MaterialHandle_t i = Interfaces::MaterialSystem->FirstMaterial(); i != Interfaces::MaterialSystem->InvalidMaterial(); i = Interfaces::MaterialSystem->NextMaterial(i))
					{
						IMaterial *pMaterial = Interfaces::MaterialSystem->GetMaterial(i);

						if (!pMaterial)
							continue;

						if (strstr(pMaterial->GetTextureGroupName(), "World")) {
							//	pMaterial->AlphaModulate(0 / 255);
							pMaterial->ColorModulate(1, 1, 1);
							memes = true;
						}

					}
				}

			}
		if (Menu::Window.VisualsTab.OtherWorld.GetIndex() == 1) {
			{
				static bool memes = false;
				if (memes = false)
				{
					for (MaterialHandle_t i = Interfaces::MaterialSystem->FirstMaterial(); i != Interfaces::MaterialSystem->InvalidMaterial(); i = Interfaces::MaterialSystem->NextMaterial(i))
					{
						IMaterial *pMaterial = Interfaces::MaterialSystem->GetMaterial(i);

						if (!pMaterial)
							continue;

						if (strstr(pMaterial->GetTextureGroupName(), "World")) {
							//	pMaterial->AlphaModulate(0 / 255);
							pMaterial->ColorModulate(0.4, 0.4, 0.4);
							memes = true;
						}

					}
				}

			}
		}
		if (Menu::Window.VisualsTab.OtherWorld.GetIndex() == 2) {
			{
				static bool memes = false;
				if (memes = false)
				{
					for (MaterialHandle_t i = Interfaces::MaterialSystem->FirstMaterial(); i != Interfaces::MaterialSystem->InvalidMaterial(); i = Interfaces::MaterialSystem->NextMaterial(i))
					{
						IMaterial *pMaterial = Interfaces::MaterialSystem->GetMaterial(i);

						if (!pMaterial)
							continue;

						if (strstr(pMaterial->GetTextureGroupName(), "World")) {
							//	pMaterial->AlphaModulate(0 / 255);
							pMaterial->ColorModulate(0.1, 0.1, 0.4);
							memes = true;
						}

					}
				}

			}
		}
	
		if (bSendPacket)
			LastAngleAA = pCmd->viewangles;
		if (!bSendPacket)
			LastAngleAAFake = pCmd->viewangles;
		plist.UpdateSim();
	}

	return false;
}


extern int hitmarkertime;

// Paint Traverse Hooked function
void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	if (Menu::Window.VisualsTab.OtherNoScope.GetState() && !strcmp("HudZoom", Interfaces::Panels->GetName(vguiPanel)))
		return;
	
	oPaintTraverse(pPanels, vguiPanel, forceRepaint, allowForce);

	static unsigned int FocusOverlayPanel = 0;
	static bool FoundPanel = false;

	if (!FoundPanel)
	{
		PCHAR szPanelName = (PCHAR)Interfaces::Panels->GetName(vguiPanel);
		if (strstr(szPanelName, "MatSystemTopPanel"))
		{
			FocusOverlayPanel = vguiPanel;
			FoundPanel = true;
		}
	}
	else if (FocusOverlayPanel == vguiPanel)
	{	


		
		if (Menu::Window.MiscTab.OtherHitmarker.GetIndex() > 0)
		{
			if (hitmarkertime > 0)
			{

				RECT View = Render::GetViewport();
				int MidX = View.right / 2;
				int MidY = View.bottom / 2;

				float alpha = hitmarkertime;
				IGameEvent* pEvent;

				Render::Line(MidX - 20, MidY - 20, MidX + 20, MidY + 20, Color(255, 255, 255, alpha));
				Render::Line(MidX + 20, MidY - 20, MidX - 20, MidY + 20, Color(255, 255, 255, alpha));

				hitmarkertime -= 2;
			}

		}









		
		
	
	


		static float rainbow;
		rainbow += 0.005f;
		if (rainbow > 1.f) rainbow = 0.f;

		Render::Text(80, 32, Color(0, 255, 0, 255), Render::Fonts::Watermark , "SUICIDE.CC");
		Render::Text(80, 32, Color(255, 255, 255, 255), Render::Fonts::Watermark, "SUICIDE.");
		Render::Text(10, 15, Color(255, 255, 255, 255), Render::Fonts::Tab, "A");
		// Render::Text(10, 10, Color::FromHSB(rainbow, 1.f, 1.f), Render::Fonts::Menu, "WarmHook - Build 2.0");
		// Render::Text(10, 25, Color(66, 210, 255, 220), Render::Fonts::Menu, "Made for ENVORIAL.WIN");

		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
			Hacks::DrawHacks();

		// Update and draw the menu
		Menu::DoUIFrame();
	}
}

// InPrediction Hooked Function
bool __stdcall Hooked_InPrediction()
{
	bool result;
	static InPrediction_ origFunc = (InPrediction_)Hooks::VMTPrediction.GetOriginalFunction(14);
	static DWORD *ecxVal = Interfaces::Prediction;
	result = origFunc(ecxVal);

	// If we are in the right place where the player view is calculated
	// Calculate the change in the view and get rid of it
	if (Menu::Window.VisualsTab.OtherNoVisualRecoil.GetState() && (DWORD)(_ReturnAddress()) == Offsets::Functions::dwCalcPlayerView)
	{
		IClientEntity* pLocalEntity = NULL;

		float* m_LocalViewAngles = NULL;

		__asm
		{
			MOV pLocalEntity, ESI
			MOV m_LocalViewAngles, EBX
		}

		Vector viewPunch = pLocalEntity->localPlayerExclusive()->GetViewPunchAngle();
		Vector aimPunch = pLocalEntity->localPlayerExclusive()->GetAimPunchAngle();

		m_LocalViewAngles[0] -= (viewPunch[0] + (aimPunch[0] * 2 * 0.4499999f));
		m_LocalViewAngles[1] -= (viewPunch[1] + (aimPunch[1] * 2 * 0.4499999f));
		m_LocalViewAngles[2] -= (viewPunch[2] + (aimPunch[2] * 2 * 0.4499999f));
		return true;
	}

	return result;
}

// DrawModelExec for chams and shit
void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	Color color;
	float flColor[3] = { 0.f };
	static IMaterial* CoveredLit = CreateMaterial(true);
	static IMaterial* OpenLit = CreateMaterial(false);
	static IMaterial* CoveredFlat = CreateMaterial(true, false);
	static IMaterial* OpenFlat = CreateMaterial(false, false);
	bool DontDraw = false;

	const char* ModelName = Interfaces::ModelInfo->GetModelName((model_t*)pInfo.pModel);
	IClientEntity* pModelEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(pInfo.entity_index);
	IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Menu::Window.VisualsTab.Active.GetState())
	{
		// Player Chams
		int ChamsStyle = Menu::Window.VisualsTab.OptionsChams.GetIndex();
		int HandsStyle = Menu::Window.VisualsTab.OtherNoHands.GetIndex();
		if (ChamsStyle != 0 && Menu::Window.VisualsTab.FiltersPlayers.GetState() && strstr(ModelName, "models/player"))
		{
			if (pLocal /* && (!Menu::Window.VisualsTab.FiltersEnemiesOnly.GetState() || pModelEntity->GetTeamNum() != pLocal->GetTeamNum())*/)
			{
				IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
				IMaterial *open = ChamsStyle == 1 ? OpenLit : OpenFlat;

				IClientEntity* pModelEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(pInfo.entity_index);
				if (pModelEntity)
				{
					IClientEntity *local = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
					if (local)
					{
						if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0 /*&& pModelEntity->GetTeamNum() != local->GetTeamNum()*/)
						{
							float alpha = 1.f;

							if (pModelEntity->HasGunGameImmunity())
								alpha = 0.5f;

							if (pModelEntity->GetTeamNum() == 2)
							{
								flColor[0] = 240.f / 255.f;
								flColor[1] = 30.f / 255.f;
								flColor[2] = 35.f / 255.f;
							}
							else
							{
								flColor[0] = 63.f / 255.f;
								flColor[1] = 72.f / 255.f;
								flColor[2] = 205.f / 255.f;
							}

							Interfaces::RenderView->SetColorModulation(flColor);
							Interfaces::RenderView->SetBlend(alpha);
							Interfaces::ModelRender->ForcedMaterialOverride(covered);
							oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

							if (pModelEntity->GetTeamNum() == 2)
							{
								flColor[0] = 247.f / 255.f;
								flColor[1] = 180.f / 255.f;
								flColor[2] = 20.f / 255.f;
							}
							else
							{
								flColor[0] = 32.f / 255.f;
								flColor[1] = 180.f / 255.f;
								flColor[2] = 57.f / 255.f;
							}

							Interfaces::RenderView->SetColorModulation(flColor);
							Interfaces::RenderView->SetBlend(alpha);
							Interfaces::ModelRender->ForcedMaterialOverride(open);
						}
						else
						{
							color.SetColor(255, 255, 255, 255);
							ForceMaterial(color, open);
						}
					}
				}
			}
		}
		else if (HandsStyle != 0 && strstr(ModelName, "arms"))
		{
			if (HandsStyle == 1)
			{
				DontDraw = true;
			}
			else if (HandsStyle == 2)
			{
				Interfaces::RenderView->SetBlend(0.3);
			}
			else if (HandsStyle == 3)
			{
				IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
				IMaterial *open = ChamsStyle == 1 ? OpenLit : OpenFlat;
				if (pLocal)
				{
					if (pLocal->IsAlive())
					{
						int alpha = pLocal->HasGunGameImmunity() ? 150 : 255;

						if (pLocal->GetTeamNum() == 2)
							color.SetColor(240, 30, 35, alpha);
						else
							color.SetColor(63, 72, 205, alpha);

						ForceMaterial(color, covered);
						oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

						if (pLocal->GetTeamNum() == 2)
							color.SetColor(247, 180, 20, alpha);
						else
							color.SetColor(32, 180, 57, alpha);
					}
					else
					{
						color.SetColor(255, 255, 255, 255);
					}

					ForceMaterial(color, open);
				}
			}
			else
			{
				static int counter = 0;
				static float colors[3] = { 1.f, 0.f, 0.f };

				if (colors[counter] >= 1.0f)
				{
					colors[counter] = 1.0f;
					counter += 1;
					if (counter > 2)
						counter = 0;
				}
				else
				{
					int prev = counter - 1;
					if (prev < 0) prev = 2;
					colors[prev] -= 0.05f;
					colors[counter] += 0.05f;
				}

				Interfaces::RenderView->SetColorModulation(colors);
				Interfaces::RenderView->SetBlend(0.3);
				Interfaces::ModelRender->ForcedMaterialOverride(OpenLit);
			}
		}
		else if (ChamsStyle != 0 && Menu::Window.VisualsTab.FiltersWeapons.GetState() && strstr(ModelName, "_dropped.mdl"))
		{
			IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
			color.SetColor(255, 255, 255, 255);
			ForceMaterial(color, covered);
		}
	}

	if (!DontDraw)
		oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
	Interfaces::ModelRender->ForcedMaterialOverride(NULL);
}



void AutoResolver(Vector* & Angle, PlayerList::CPlayer* Player)
{
	/*if (Settings.GetMenuSetting(Tab_Misc, Misc_Aimstep))
	{
	string s = "echo " + std::to_string(Hacks.CurrentCmd->tick_count) + ":" + std::to_string(Player->entity->pelvisangs());
	Interfaces.pEngine->ClientCmd_Unrestricted(s.c_str(), 0);
	}*/

	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (flLowerBodyYaw != Player->flLastPelvisAng)
		{
			if (Player->entity->GetVecVelocity().Length2D() == 0)
			{
				int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
				while (temp < 0)
					temp += 360;
				while (temp > 360)
					temp -= 360;
				Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
			}

			iLastUpdatedTick = 0;
			Player->flLastPelvisAng = flLowerBodyYaw;
			Player->flEyeAng = orginalview.y;
		}

		if (Player->entity->GetVecVelocity().Length2D() >= 1)
		{
			flResolve = flLowerBodyYaw;
		}
		else
		{
			flResolve = Player->flLastPelvisAng;
		}
		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}




// Hooked FrameStageNotify for removing visual recoil
void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage)
{
	DWORD eyeangles = NetVar.GetNetVar(0xBFEA4E7B);
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_RENDER_START)
	{
		
		if (Menu::Window.VisualsTab.OtherThirdpersonAngle.GetIndex() == 0 && pLocal->IsAlive())
		{
			if (*(bool*)((DWORD)Interfaces::pInput + 0xA5))
				*(Vector*)((DWORD)pLocal + 0x31C8) = LastAngleAA;
		}
		if (Menu::Window.VisualsTab.OtherThirdpersonAngle.GetIndex() == 1 && pLocal->IsAlive())
		{
			if (*(bool*)((DWORD)Interfaces::pInput + 0xA5))
				*(Vector*)((DWORD)pLocal + 0x31C8) = LastAngleAAFake;
		}
		

	/*	if (Menu::Window.VisualsTab.OtherThirdperson.GetState())
		{
			if(pLocal->IsAlive())
			{
				*(bool*)((DWORD)Interfaces::pInput + 0xA5) = true;
				*(float*)((DWORD)Interfaces::pInput + 0xA8 + 0x8) = 63;
			}
			else
			{
				*(bool*)((DWORD)Interfaces::pInput + 0xA5) = false;
				*(float*)((DWORD)Interfaces::pInput + 0xA8 + 0x8) = 0;
			}
			
		}
		if (!Menu::Window.VisualsTab.OtherThirdperson.GetState())
		{
			*(bool*)((DWORD)Interfaces::pInput + 0xA5) = false;
			*(float*)((DWORD)Interfaces::pInput + 0xA8 + 0x8) = 0;
		}
		*/
		if (Menu::Window.VisualsTab.OtherThirdperson.GetState())
		{
			static bool rekt = false;
			if (!rekt)
			{
				ConVar* sv_cheats = Interfaces::CVar->FindVar("sv_cheats");
				SpoofedConvar* sv_cheats_spoofed = new SpoofedConvar(sv_cheats);
				sv_cheats_spoofed->SetInt(1);
				rekt = true;
			}
		}

		static bool rekt1 = false;
		if (Menu::Window.VisualsTab.OtherThirdperson.GetState() && pLocal->IsAlive() && pLocal->IsScoped() == 0)
		{
			if (!rekt1)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
				rekt1 = true;
			}
		}
		else if (!Menu::Window.VisualsTab.OtherThirdperson.GetState())
		{
			rekt1 = false;
		}

		static bool rekt = false;
		if (!Menu::Window.VisualsTab.OtherThirdperson.GetState() || pLocal->IsAlive() == 0)
		{
			if (!rekt)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("firstperson");
				rekt = true;
			}
		}
		else if (Menu::Window.VisualsTab.OtherThirdperson.GetState() || pLocal->IsAlive() || pLocal->IsScoped() == 0)
		{
			rekt = false;
		}

		static bool meme = false;
		if (Menu::Window.VisualsTab.OtherThirdperson.GetState() && pLocal->IsScoped() == 0)
		{
			if (!meme)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
				meme = true;
			}
		}
		else if (pLocal->IsScoped())
		{
			meme = false;
		}

		static bool kek = false;
		if (Menu::Window.VisualsTab.OtherThirdperson.GetState() && pLocal->IsAlive())
		{
			if (!kek)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
				kek = true;
			}
		}
		else if (pLocal->IsAlive() == 0)
		{
			kek = false;
		}
	}

	// PVS Fix
	if (curStage == FRAME_RENDER_START)
	{
		for (int i = 1; i <= Interfaces::Globals->maxClients; i++)
		{
			if (Menu::Window.RageBotTab.PVSFix.GetState()) {


				if (i == Interfaces::Engine->GetLocalPlayer()) continue;

				IClientEntity* pMemeEnt = Interfaces::EntList->GetClientEntity(i);
				if (!pMemeEnt) continue;

				*(int*)((uintptr_t)pMemeEnt + 0xA30) = Interfaces::Globals->framecount; //we'll skip occlusion checks now
				*(int*)((uintptr_t)pMemeEnt + 0xA28) = 0;//clear occlusion flags
			}
		}
	}

	// Skybox Changer
	if (Interfaces::Engine->IsInGame())
	{
		ConVar* skyboxmeme = Interfaces::CVar->FindVar("sv_skyname");
		if (Menu::Window.VisualsTab.CustomSky.GetState())
		{
			skyboxmeme->SetValue("sky_csgo_night02");
		}
		else
		{
			skyboxmeme->SetValue("sky_csgo_night02b");
		}
	}
	plist.Update();
	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START && Menu::Window.RageBotTab.AccuracyResolverAuto.GetIndex() > 0 || Menu::Window.RageBotTab.AccuracyResolverPitch.GetIndex() > 0 || Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() > 0)
	{

		if (Menu::Window.RageBotTab.AccuracyResolverAuto.GetIndex() == 1) // Test 1
		{
			if (Interfaces::Engine->IsInGame() && Interfaces::Engine->IsConnected())
			{
				for (auto i = 0; i <= Interfaces::EntList->GetHighestEntityIndex(); i++)
				{
					auto pEntity = static_cast<IClientEntity*>(Interfaces::EntList->GetClientEntity(i));
					if (pEntity == nullptr)
						continue;
					if (pEntity == pLocal)
						continue;
					if (!pLocal->IsAlive())
						continue;
					if (pEntity->GetTeam() == pLocal->GetTeam())
						continue;
					if (!pEntity->IsAlive())
						continue;
					if (!(pEntity->GetHealth() > 0))
						continue;
					if (pEntity->IsDormant())
						continue;
					player_info_t info;
					if (!(Interfaces::Engine->GetPlayerInfo(pEntity->GetIndex(), &info)))
						continue;
					PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
					Player->entity = pEntity;
					Vector* angs = Player->entity->GetEyeAnglesPointer();
					if (!Menu::Window.MiscTab.OtherSafeMode.GetState())
					{
						if (angs->x > 90.0f && angs->x < 270.f)
						{
							if (angs->x > 180.f)
								angs->x = -89.f;
							else
								angs->x = 89.0f;
						}
						//angs->y = spin;
					}
					AutoResolver(angs, Player);
				}
			}
		}
	}
	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) 
	{
		if (pLocal->IsAlive())
		{
			R::Resolver();
		}
		
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		if (Menu::Window.SkinTab.SkinEnable.GetState() && pLocal)
		{
		IClientEntity* WeaponEnt = Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

		int iBayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
		int iButterfly = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
		int iFlip = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
		int iGut = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
		int iKarambit = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
		int iM9Bayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
		int iHuntsman = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
		int iFalchion = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
		int iDagger = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
		int iBowie = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");

		int iGunGame = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gg.mdl");

		for (int i = 0; i <= Interfaces::EntList->GetHighestEntityIndex(); i++)
		{
			IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);

			IClientEntity* WeaponEnt = Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

			CBaseCombatWeapon* Weapon = (CBaseCombatWeapon*)WeaponEnt;

			if (pEntity)
			{
				ULONG hOwnerEntity = *(PULONG)((DWORD)pEntity + 0x148);

				IClientEntity* pOwner = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)hOwnerEntity);

				if (pOwner)
				{
					if (pOwner == pLocal)
					{
						std::string sWeapon = Interfaces::ModelInfo->GetModelName(pEntity->GetModel());

						if (!(sWeapon.find("models/weapons", 0) != std::string::npos))
							continue;

						if (sWeapon.find("c4_planted", 0) != std::string::npos)
							continue;

						if (sWeapon.find("thrown", 0) != std::string::npos)
							continue;

						if (sWeapon.find("smokegrenade", 0) != std::string::npos)
							continue;

						if (sWeapon.find("flashbang", 0) != std::string::npos)
							continue;

						if (sWeapon.find("fraggrenade", 0) != std::string::npos)
							continue;

						if (sWeapon.find("molotov", 0) != std::string::npos)
							continue;

						if (sWeapon.find("decoy", 0) != std::string::npos)
							continue;

						if (sWeapon.find("incendiarygrenade", 0) != std::string::npos)
							continue;

						if (sWeapon.find("ied", 0) != std::string::npos)
							continue;

						if (sWeapon.find("w_eq_", 0) != std::string::npos)
							continue;

						CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)pEntity;

						ClientClass *pClass = Interfaces::Client->GetAllClasses();









						if (Menu::Window.SkinTab.GloveActive.GetState())
						{
							IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

							static bool bUpdate = false;

							int* hMyWearables = pLocal->GetWearables();

							if (!hMyWearables)
								return;

							if (!Interfaces::EntList->GetClientEntity(hMyWearables[0] & 0xFFF))
							{
								for (ClientClass *pClass = Interfaces::Client->GetAllClasses(); pClass; pClass = pClass->m_pNext)
								{
									if (pClass->m_ClassID != (int)CSGOClassID::CEconWearable)
										continue;

									int entry = (Interfaces::EntList->GetHighestEntityIndex() + 1), serial = RandomInt(0x0, 0xFFF);
									pClass->m_pCreateFn(entry, serial);
									hMyWearables[0] = entry | (serial << 16); //crash

									bUpdate = true;
									break;
								}


								player_info_t LocalPlayerInfo;
								Interfaces::Engine->GetPlayerInfo(Interfaces::Engine->GetLocalPlayer(), &LocalPlayerInfo);
							}
							CBaseCombatWeapon* pWeapon_g = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntity(pLocal->GetWearables()[0] & 0xFFF);



							int Glove_model = Menu::Window.SkinTab.GloveModel.GetIndex();
							int Glove_skin = Menu::Window.SkinTab.GloveSkin.GetIndex();

							if (!pWeapon_g)
								return;

							if (bUpdate) {
								if (Glove_model == 0)
								{

									*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
									*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;

									((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
									pWeapon_g->PreDataUpdate(0);

									if (Glove_skin != 0 && Glove_skin != 1 && Glove_skin != 2)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10006;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 0)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10006;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 1)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10007;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 2)
									{
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10008;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}

								}
								if (Glove_model == 1)
								{

									*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
									*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
									((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
									pWeapon_g->PreDataUpdate(0);

									if (Glove_skin != 3 && Glove_skin != 4 && Glove_skin != 5 && Glove_skin != 6)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10037;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 3)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10037;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}

									else if (Glove_skin == 4)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10038;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}

									else if (Glove_skin == 5)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10018;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}

									else if (Glove_skin == 6)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10019;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
								}
								if (Glove_model == 2)
								{

									*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
									*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
									((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
									pWeapon_g->PreDataUpdate(0);

									if (Glove_skin != 7 && Glove_skin != 8 && Glove_skin != 9)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5031;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10013;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 7)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5031;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10013;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 8)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5031;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10015;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 9)
									{


										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5031;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10016;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
								}
								if (Glove_model == 3)
								{

									*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
									*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
									((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
									pWeapon_g->PreDataUpdate(0);
									if (Glove_skin != 10 && Glove_skin != 11 && Glove_skin != 12 && Glove_skin != 13)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10030;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 10)
									{


										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10030;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 11)
									{


										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10033;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 12)
									{


										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10034;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 13)
									{


										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10035;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
								}
								if (Glove_model == 4)
								{

									*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
									*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
									((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
									pWeapon_g->PreDataUpdate(0);
									if (Glove_skin != 14 && Glove_skin != 15 && Glove_skin != 16 && Glove_skin != 17)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10024;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 14)
									{


										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10026;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 15)
									{


										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10027;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 16)
									{


										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10028;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 17)
									{


										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10024;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
								}
								if (Glove_model == 5)
								{

									*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
									*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
									((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
									pWeapon_g->PreDataUpdate(0);
									if (Glove_skin != 18 && Glove_skin != 19 && Glove_skin != 20 && Glove_skin != 21)
									{

										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10009;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 18)
									{


										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10009;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 19)
									{


										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10010;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 20)
									{


										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10021;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
									else if (Glove_skin == 21)
									{


										*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
										*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
										*pWeapon_g->FallbackPaintKit() = 10036;
										((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
										pWeapon_g->PreDataUpdate(0);
									}
								}

								/*	*pWeapon_g->ModelIndex() = sporty; // m_nModelIndex
								*pWeapon_g->ViewModelIndex() = sporty;
								*pWeapon_g->WorldModelIndex() = sporty + 1;
								*pWeapon_g->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
								*pWeapon_g->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
								*pWeapon_g->FallbackPaintKit() = 10018;
								((IClientEntity*)pWeapon_g)->SetModelIndexVirtual(Interfaces::ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
								pWeapon_g->PreDataUpdate(0);*/

							}
						}







						if (Menu::Window.SkinTab.SkinEnable.GetState())
						{
							int Model = Menu::Window.SkinTab.KnifeModel.GetIndex();

							int M41S = Menu::Window.SkinTab.M41SSkin.GetIndex();
							int M4A4 = Menu::Window.SkinTab.M4A4Skin.GetIndex();
							int AK47 = Menu::Window.SkinTab.AK47Skin.GetIndex();
							int AWP = Menu::Window.SkinTab.AWPSkin.GetIndex();
							int GLOCK = Menu::Window.SkinTab.GLOCKSkin.GetIndex();
							int USPS = Menu::Window.SkinTab.USPSSkin.GetIndex();
							int DEAGLE = Menu::Window.SkinTab.DEAGLESkin.GetIndex();
							int FIVE7 = Menu::Window.SkinTab.FIVESEVENSkin.GetIndex();
							int AUG = Menu::Window.SkinTab.AUGSkin.GetIndex();
							int FAMAS = Menu::Window.SkinTab.FAMASSkin.GetIndex();
							int G3SG1 = Menu::Window.SkinTab.G3SG1Skin.GetIndex();
							int Galil = Menu::Window.SkinTab.GALILSkin.GetIndex();
							int M249 = Menu::Window.SkinTab.M249Skin.GetIndex();
							int MAC10 = Menu::Window.SkinTab.MAC10Skin.GetIndex();
							int P90 = Menu::Window.SkinTab.P90Skin.GetIndex();
							int UMP45 = Menu::Window.SkinTab.UMP45Skin.GetIndex();
							int XM1014 = Menu::Window.SkinTab.XM1014Skin.GetIndex();
							int BIZON = Menu::Window.SkinTab.BIZONSkin.GetIndex();
							int MAG7 = Menu::Window.SkinTab.MAG7Skin.GetIndex();
							int NEGEV = Menu::Window.SkinTab.NEGEVSkin.GetIndex();
							int SAWEDOFF = Menu::Window.SkinTab.SAWEDOFFSkin.GetIndex();
							int TEC9 = Menu::Window.SkinTab.TECNINESkin.GetIndex();
							int P2000 = Menu::Window.SkinTab.P2000Skin.GetIndex();
							int MP7 = Menu::Window.SkinTab.MP7Skin.GetIndex();
							int MP9 = Menu::Window.SkinTab.MP9Skin.GetIndex();
							int NOVA = Menu::Window.SkinTab.NOVASkin.GetIndex();
							int P250 = Menu::Window.SkinTab.P250Skin.GetIndex();
							int SCAR20 = Menu::Window.SkinTab.SCAR20Skin.GetIndex();
							int SG553 = Menu::Window.SkinTab.SG553Skin.GetIndex();
							int SSG08 = Menu::Window.SkinTab.SSG08Skin.GetIndex();
							int Magnum = Menu::Window.SkinTab.DEAGLESkin.GetIndex();
							int DUAL = Menu::Window.SkinTab.DUALSSkin.GetIndex();


							int weapon = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();

							switch (weapon)
							{
							case 7: // AK47 
							{
								switch (AK47)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 341;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 14;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 44;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 172;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 180;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 394;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 300;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 226;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 282;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 302;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 316;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 340;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 380;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 422;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 456;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 474;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 490;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 506;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 524;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 600;
									break;
								default:
									break;
								}
							}
							break;
							case 16: // M4A4
							{
								switch (M4A4)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 155;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 187;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 255;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 309;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 215;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 336;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 384;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 400;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 449;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 471;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 480;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 512;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 533;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 588;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 632;
									break;
								default:
									break;
								}
							}
							break;
							case 60: // M4A1-S
							{
								switch (M41S)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 60;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 430;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 77;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 235;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 254;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 189;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 301;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 217;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 257;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 321;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 326;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 360;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 383;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 440;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 445;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 497;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 548;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 587;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 631;
									break;
								default:
									break;
								}
							}
							break;
							case 9: // Awp
							{
								switch (AWP)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 174;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 344;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 84;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 30;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 51;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 72;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 181;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 259;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 395;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 212;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 227;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 251;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 279;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 424;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 446;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 451;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 475;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 525;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 640;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 662;
									break;
								default:
									break;
								}
							}
							break;
							case 61: // Usp-s
							{
								switch (USPS)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 25;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 60;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 235;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 183;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 339;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 217;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 221;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 236;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 277;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 290;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 313;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 318;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 332;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 364;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 454;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 489;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 504;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 540;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 637;
									break;
								default:
									break;
								}
							}
							break;
							case 4: // Glock
							{
								switch (GLOCK)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 2;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 3;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 38;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 40;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 48;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 437;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 99;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 159;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 399;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 208;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 230;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 278;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 293;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 353;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 367;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 381;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 479;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 495;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 532;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 607;
									break;
								case 20:
									*pWeapon->FallbackPaintKit() = 586;
									break;
								default:
									break;
								}
							}
							break;
							case 1: // Deagle
							{
								switch (DEAGLE)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 37;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 347;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 468;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 469;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 5;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 232;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 17;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 40;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 61;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 90;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 235;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 185;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 231;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 237;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 397;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 328;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 273;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 296;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 351;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 425;
									break;
								case 20:
									*pWeapon->FallbackPaintKit() = 470;
									break;
								case 21:
									*pWeapon->FallbackPaintKit() = 509;
									break;
								case 22:
									*pWeapon->FallbackPaintKit() = 527;
									break;
								default:
									break;
								}
							}
							break;
							case 2: // Dual Berettas
							{
								switch (DUAL)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 28;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 36;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 43;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 46;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 47;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 153;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 491;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 190;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 248;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 249;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 220;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 396;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 261;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 276;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 307;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 330;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 447;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 450;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 528;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 544;
									break;
								case 20:
									*pWeapon->FallbackPaintKit() = 625;
									break;
								case 21:
									*pWeapon->FallbackPaintKit() = 658;
									break;
								default:
									break;
								}
							}
							break;
							case 3: // Five Seven
							{
								switch (FIVE7)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 3;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 27;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 44;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 46;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 78;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 141;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 151;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 254;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 248;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 210;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 223;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 252;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 265;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 274;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 464;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 352;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 377;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 387;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 427;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 510;
									break;
								case 20:
									*pWeapon->FallbackPaintKit() = 530;
									break;
								case 21: // Capillary
									*pWeapon->FallbackPaintKit() = 646;
									break;
								case 22: // Hyper Beast
									*pWeapon->FallbackPaintKit() = 660;
									break;
								default:
									break;
								}
							}
							break;
							case 8: // AUG
							{
								switch (AUG)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 9;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 33;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 280;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 305;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 375;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 442;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 444;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 455;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 507;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 541;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 601;
									break;
								default:
									break;
								}
							}
							break;
							case 10: // Famas
							{
								switch (FAMAS)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 22;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 47;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 92;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 429;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 154;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 178;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 194;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 244;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 218;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 260;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 288;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 371;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 477;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 492;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 529;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 604;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 626;
									break;
								default:
									break;
								}
							}
							break;
							case 11: // G3SG1
							{
								switch (G3SG1)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 8;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 6;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 27;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 46;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 72;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 74;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 147;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 235;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 170;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 195;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 229;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 294;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 465;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 464;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 382;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 438;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 493;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 511;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 545;
									break;
								default:
									break;
								}
							}
							break;
							case 13: // Galil
							{
								switch (Galil)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 5;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 22;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 83;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 428;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 76;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 119;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 235;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 235;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 398;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 192;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 308;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 216;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 237;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 241;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 264;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 297;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 379;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 460;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 478;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 494;
									break;
								case 20:
									*pWeapon->FallbackPaintKit() = 546;
									break;
								default:
									break;
								}
							}
							break;
							case 14: // M249
							{
								switch (M249)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 22;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 75;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 202;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 243;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 266;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 401;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 452;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 472;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 496;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 547;
									break;
								default:
									break;
								}
							}
							break;
							case 17: // Mac 10
							{
								switch (MAC10)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 101;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 3;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 32;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 5;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 17;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 38;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 433;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 98;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 157;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 188;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 337;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 246;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 284;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 310;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 333;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 343;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 372;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 402;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 498;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 534;
									break;
								default:
									break;
								}
							}
							break;
							case 19: // P90
							{
								switch (P90)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 342;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 20;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 22;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 100;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 67;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 111;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 124;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 156;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 234;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 169;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 175;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 182;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 244;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 228;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 283;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 311;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 335;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 359;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 486;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 516;
									break;
								case 20:
									*pWeapon->FallbackPaintKit() = 636;
									break;
								default:
									break;
								}
							}
							break;
							case 24: // UMP-45
							{
								switch (UMP45)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 37;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 5;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 15;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 17;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 436;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 70;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 93;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 169;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 175;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 193;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 392;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 281;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 333;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 362;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 441;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 488;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 556;
									break;
								default:
									break;
								}
							}
							break;
							case 25: // XM1014
							{
								switch (XM1014)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 166;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 238;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 27;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 42;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 96;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 95;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 135;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 151;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 235;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 235;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 169;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 205;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 240;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 251;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 393;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 320;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 314;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 348;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 370;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 407;
									break;
								case 20:
									*pWeapon->FallbackPaintKit() = 505;
									break;
								case 21:
									*pWeapon->FallbackPaintKit() = 521;
									break;
								case 22:
									*pWeapon->FallbackPaintKit() = 557;
									break;
								default:
									break;
								}
							}
							break;
							case 26: // Bizon
							{
								switch (BIZON)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 13;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 164;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 25;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 27;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 70;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 148;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 149;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 159;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 235;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 171;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 203;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 224;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 236;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 267;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 306;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 323;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 349;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 376;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 457;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 459;
									break;
								case 20:
									*pWeapon->FallbackPaintKit() = 508;
									break;
								case 21:
									*pWeapon->FallbackPaintKit() = 526;
									break;
								case 22:
									*pWeapon->FallbackPaintKit() = 542;
									break;
								default:
									break;
								}
							}
							break;
							case 27: // Mag 7
							{
								switch (MAG7)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 462;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 34;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 32;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 100;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 39;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 431;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 99;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 171;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 177;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 198;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 291;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 385;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 473;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 499;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 535;
									break;
								default:
									break;
								}
							}
							break;
							case 28: // Negev
							{
								switch (NEGEV)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 28;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 432;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 157;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 235;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 201;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 240;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 285;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 298;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 317;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 355;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 369;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 483;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 514;
									break;
								default:
									break;
								}
							}
							break;
							case 29: // Sawed Off
							{
								switch (SAWEDOFF)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 345;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 5;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 22;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 30;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 83;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 38;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 41;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 434;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 119;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 235;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 171;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 204;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 405;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 246;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 250;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 390;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 256;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 323;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 458;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 459;
									break;
								case 20:
									*pWeapon->FallbackPaintKit() = 517;
									break;
								case 21:
									*pWeapon->FallbackPaintKit() = 552;
									break;
								case 22:
									*pWeapon->FallbackPaintKit() = 638;
									break;
								default:
									break;
								}
							}
							break;
							case 30: // Tec 9
							{
								switch (TEC9)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 101;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 2;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 5;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 463;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 17;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 36;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 439;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 159;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 235;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 179;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 248;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 206;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 216;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 242;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 272;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 289;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 303;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 374;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 459;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 520;
									break;
								case 20:
									*pWeapon->FallbackPaintKit() = 539;
									break;
								case 21:
									*pWeapon->FallbackPaintKit() = 555;
									break;
								case 22:
									*pWeapon->FallbackPaintKit() = 614;
									break;
								default:
									break;
								}
							}
							break;
							case 32: // P2000
							{
								switch (P2000)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 104;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 32;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 21;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 25;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 36;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 485;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 38;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 71;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 95;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 184;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 211;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 338;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 246;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 275;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 327;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 346;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 357;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 389;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 442;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 443;
									break;
								case 20:
									*pWeapon->FallbackPaintKit() = 515;
									break;
								case 21:
									*pWeapon->FallbackPaintKit() = 550;
									break;
								case 22:
									*pWeapon->FallbackPaintKit() = 591;
									break;
								default:
									break;
								}
							}
							break;
							case 33: // MP7
							{
								switch (MP7)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 2;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 102;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 5;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 28;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 11;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 15;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 22;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 27;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 36;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 141;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 235;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 245;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 209;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 213;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 250;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 354;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 365;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 423;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 442;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 481;
									break;
								case 20:
									*pWeapon->FallbackPaintKit() = 500;
									break;
								case 21:
									*pWeapon->FallbackPaintKit() = 536;
									break;
								default:
									break;
								}
							}
							break;
							case 34: // MP9
							{
								switch (MP9)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 482;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 27;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 33;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 100;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 39;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 61;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 148;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 141;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 199;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 329;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 262;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 366;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 368;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 386;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 403;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 448;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 549;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 609;
									break;
								default:
									break;
								}
							}
							break;
							case 35: // Nova
							{
								switch (NOVA)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 3;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 166;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 164;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 25;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 62;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 99;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 107;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 158;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 170;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 191;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 214;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 225;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 263;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 286;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 294;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 299;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 356;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 450;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 484;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 537;
									break;
								default:
									break;
								}
							}
							break;
							case 36: // P250
							{
								switch (P250)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 102;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 34;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 162;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 15;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 164;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 27;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 77;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 99;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 168;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 258;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 207;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 219;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 404;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 230;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 271;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 295;
									break;
								case 16:
									*pWeapon->FallbackPaintKit() = 464;
									break;
								case 17:
									*pWeapon->FallbackPaintKit() = 358;
									break;
								case 18:
									*pWeapon->FallbackPaintKit() = 373;
									break;
								case 19:
									*pWeapon->FallbackPaintKit() = 388;
									break;
								case 20:
									*pWeapon->FallbackPaintKit() = 426;
									break;
								case 21:
									*pWeapon->FallbackPaintKit() = 466;
									break;
								case 22:
									*pWeapon->FallbackPaintKit() = 467;
									break;
								case 23:
									*pWeapon->FallbackPaintKit() = 501;
									break;
								case 24:
									*pWeapon->FallbackPaintKit() = 551;
									break;
								default:
									break;
								}
							}
							break;
							case 38: // Scar 20
							{
								switch (SCAR20)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 165;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 100;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 46;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 70;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 116;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 157;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 196;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 232;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 391;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 298;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 312;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 406;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 453;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 502;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 518;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 597;
									break;
								default:
									break;
								}
							}
							break;
							case 39: // SG553
							{
								switch (SG553)
								{
								case 1:
									*pWeapon->FallbackPaintKit() = 39;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 98;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 410;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 347;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 287;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 298;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 363;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 378;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 487;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 519;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 553;
									break;
								default:
									break;
								}
							}
							break;
							case 40: // SSG08
							{
								switch (SSG08)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 26;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 60;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 96;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 99;
									break;
								case 4:
									*pWeapon->FallbackPaintKit() = 157;
									break;
								case 5:
									*pWeapon->FallbackPaintKit() = 200;
									break;
								case 6:
									*pWeapon->FallbackPaintKit() = 222;
									break;
								case 7:
									*pWeapon->FallbackPaintKit() = 233;
									break;
								case 8:
									*pWeapon->FallbackPaintKit() = 253;
									break;
								case 9:
									*pWeapon->FallbackPaintKit() = 304;
									break;
								case 10:
									*pWeapon->FallbackPaintKit() = 319;
									break;
								case 11:
									*pWeapon->FallbackPaintKit() = 361;
									break;
								case 12:
									*pWeapon->FallbackPaintKit() = 503;
									break;
								case 13:
									*pWeapon->FallbackPaintKit() = 538;
									break;
								case 14:
									*pWeapon->FallbackPaintKit() = 554;
									break;
								case 15:
									*pWeapon->FallbackPaintKit() = 624;
									break;
								default:
									break;
								}
							}
							break;
							case 64: // Revolver
							{
								switch (Magnum)
								{
								case 0:
									*pWeapon->FallbackPaintKit() = 27;
									break;
								case 1:
									*pWeapon->FallbackPaintKit() = 12;
									break;
								case 2:
									*pWeapon->FallbackPaintKit() = 522;
									break;
								case 3:
									*pWeapon->FallbackPaintKit() = 523;
									break;
								default:
									break;
								}
							}
							break;
							default:
								break;
							}



							if (pEntity->GetClientClass()->m_ClassID == (int)CSGOClassID::CKnife)
							{
								if (Model == 0) // Bayonet
								{
									*pWeapon->ModelIndex() = iBayonet; // m_nModelIndex
									*pWeapon->ViewModelIndex() = iBayonet;
									*pWeapon->WorldModelIndex() = iBayonet + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 500;

									int Skin = Menu::Window.SkinTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 27; // Bone Mask
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 42; // Blue Steel
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 43; // Stained
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 44; // Case Hardened
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 143; // Urban Masked
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 175; // Scorched
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 323; // Rust Coat
									}
									else if (Skin == 15)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 16)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 17)
									{
										*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
									}
									else if (Skin == 18)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 19)
									{
										*pWeapon->FallbackPaintKit() = 414; // Rust Coat
									}
									else if (Skin == 20)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
									}
									else if (Skin == 21)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
									}
									else if (Skin == 22)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
									}
									else if (Skin == 23)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 24)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 25)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 26)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 27)
									{
										*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
									}
									else if (Skin == 28)
									{
										*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
									}
									else if (Skin == 29)
									{
										*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
									}
									else if (Skin == 30)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
									}
									else if (Skin == 31)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
									}
									else if (Skin == 32)
									{
										*pWeapon->FallbackPaintKit() = 558; // Lore
									}
								}
								else if (Model == 1) // Bowie Knife
								{
									*pWeapon->ModelIndex() = iBowie; // m_nModelIndex
									*pWeapon->ViewModelIndex() = iBowie;
									*pWeapon->WorldModelIndex() = iBowie + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 514;

									int Skin = Menu::Window.SkinTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 27; // Bone Mask
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 42; // Blue Steel
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 43; // Stained
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 44; // Case Hardened
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 143; // Urban Masked
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 175; // Scorched
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 323; // Rust Coat
									}
									else if (Skin == 15)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 16)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 17)
									{
										*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
									}
									else if (Skin == 18)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 19)
									{
										*pWeapon->FallbackPaintKit() = 414; // Rust Coat
									}
									else if (Skin == 20)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
									}
									else if (Skin == 21)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
									}
									else if (Skin == 22)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
									}
									else if (Skin == 23)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 24)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 25)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 26)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 27)
									{
										*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
									}
									else if (Skin == 28)
									{
										*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
									}
									else if (Skin == 29)
									{
										*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
									}
									else if (Skin == 30)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
									}
									else if (Skin == 31)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
									}
									else if (Skin == 32)
									{
										*pWeapon->FallbackPaintKit() = 558; // Lore
									}

								}
								else if (Model == 2) // Butterfly Knife
								{
									*pWeapon->ModelIndex() = iButterfly; // m_nModelIndex
									*pWeapon->ViewModelIndex() = iButterfly;
									*pWeapon->WorldModelIndex() = iButterfly + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 515;

									int Skin = Menu::Window.SkinTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 27; // Bone Mask
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 42; // Blue Steel
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 43; // Stained
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 44; // Case Hardened
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 143; // Urban Masked
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 175; // Scorched
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 323; // Rust Coat
									}
									else if (Skin == 15)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 16)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 17)
									{
										*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
									}
									else if (Skin == 18)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 19)
									{
										*pWeapon->FallbackPaintKit() = 414; // Rust Coat
									}
									else if (Skin == 20)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
									}
									else if (Skin == 21)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
									}
									else if (Skin == 22)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
									}
									else if (Skin == 23)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 24)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 25)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 26)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 27)
									{
										*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
									}
									else if (Skin == 28)
									{
										*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
									}
									else if (Skin == 29)
									{
										*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
									}
									else if (Skin == 30)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
									}
									else if (Skin == 31)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
									}
									else if (Skin == 32)
									{
										*pWeapon->FallbackPaintKit() = 558; // Lore
									}

								}
								else if (Model == 3) // Falchion Knife
								{
									*pWeapon->ModelIndex() = iFalchion; // m_nModelIndex
									*pWeapon->ViewModelIndex() = iFalchion;
									*pWeapon->WorldModelIndex() = iFalchion + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 512;

									int Skin = Menu::Window.SkinTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 27; // Bone Mask
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 42; // Blue Steel
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 43; // Stained
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 44; // Case Hardened
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 143; // Urban Masked
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 175; // Scorched
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 323; // Rust Coat
									}
									else if (Skin == 15)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 16)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 17)
									{
										*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
									}
									else if (Skin == 18)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 19)
									{
										*pWeapon->FallbackPaintKit() = 414; // Rust Coat
									}
									else if (Skin == 20)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
									}
									else if (Skin == 21)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
									}
									else if (Skin == 22)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
									}
									else if (Skin == 23)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 24)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 25)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 26)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 27)
									{
										*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
									}
									else if (Skin == 28)
									{
										*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
									}
									else if (Skin == 29)
									{
										*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
									}
									else if (Skin == 30)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
									}
									else if (Skin == 31)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
									}
									else if (Skin == 32)
									{
										*pWeapon->FallbackPaintKit() = 558; // Lore
									}
								}
								else if (Model == 4) // Flip Knife
								{
									*pWeapon->ModelIndex() = iFlip; // m_nModelIndex
									*pWeapon->ViewModelIndex() = iFlip;
									*pWeapon->WorldModelIndex() = iFlip + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 505;

									int Skin = Menu::Window.SkinTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 27; // Bone Mask
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 42; // Blue Steel
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 43; // Stained
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 44; // Case Hardened
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 143; // Urban Masked
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 175; // Scorched
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 323; // Rust Coat
									}
									else if (Skin == 15)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 16)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 17)
									{
										*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
									}
									else if (Skin == 18)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 19)
									{
										*pWeapon->FallbackPaintKit() = 414; // Rust Coat
									}
									else if (Skin == 20)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
									}
									else if (Skin == 21)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
									}
									else if (Skin == 22)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
									}
									else if (Skin == 23)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 24)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 25)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 26)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 27)
									{
										*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
									}
									else if (Skin == 28)
									{
										*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
									}
									else if (Skin == 29)
									{
										*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
									}
									else if (Skin == 30)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
									}
									else if (Skin == 31)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
									}
									else if (Skin == 32)
									{
										*pWeapon->FallbackPaintKit() = 559; // Lore
									}

								}
								else if (Model == 5) // Gut Knife
								{
									*pWeapon->ModelIndex() = iGut; // m_nModelIndex
									*pWeapon->ViewModelIndex() = iGut;
									*pWeapon->WorldModelIndex() = iGut + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 506;

									int Skin = Menu::Window.SkinTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 27; // Bone Mask
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 42; // Blue Steel
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 43; // Stained
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 44; // Case Hardened
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 143; // Urban Masked
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 175; // Scorched
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 323; // Rust Coat
									}
									else if (Skin == 15)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 16)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 17)
									{
										*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
									}
									else if (Skin == 18)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 19)
									{
										*pWeapon->FallbackPaintKit() = 414; // Rust Coat
									}
									else if (Skin == 20)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
									}
									else if (Skin == 21)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
									}
									else if (Skin == 22)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
									}
									else if (Skin == 23)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 24)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 25)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 26)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 27)
									{
										*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
									}
									else if (Skin == 28)
									{
										*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
									}
									else if (Skin == 29)
									{
										*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
									}
									else if (Skin == 30)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
									}
									else if (Skin == 31)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
									}
									else if (Skin == 32)
									{
										*pWeapon->FallbackPaintKit() = 560; // Lore
									}


								}
								else if (Model == 6) // Huntsman Knife
								{
									*pWeapon->ModelIndex() = iHuntsman; // m_nModelIndex
									*pWeapon->ViewModelIndex() = iHuntsman;
									*pWeapon->WorldModelIndex() = iHuntsman + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 509;

									int Skin = Menu::Window.SkinTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 27; // Bone Mask
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 42; // Blue Steel
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 43; // Stained
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 44; // Case Hardened
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 143; // Urban Masked
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 175; // Scorched
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 323; // Rust Coat
									}
									else if (Skin == 15)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 16)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 17)
									{
										*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
									}
									else if (Skin == 18)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 19)
									{
										*pWeapon->FallbackPaintKit() = 414; // Rust Coat
									}
									else if (Skin == 20)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
									}
									else if (Skin == 21)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
									}
									else if (Skin == 22)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
									}
									else if (Skin == 23)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 24)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 25)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 26)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 27)
									{
										*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
									}
									else if (Skin == 28)
									{
										*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
									}
									else if (Skin == 29)
									{
										*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
									}
									else if (Skin == 30)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
									}
									else if (Skin == 31)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
									}
									else if (Skin == 32)
									{
										*pWeapon->FallbackPaintKit() = 559; // Lore
									}


								}
								else if (Model == 7) // Karambit
								{
									*pWeapon->ModelIndex() = iKarambit; // m_nModelIndex
									*pWeapon->ViewModelIndex() = iKarambit;
									*pWeapon->WorldModelIndex() = iKarambit + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 507;

									int Skin = Menu::Window.SkinTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 27; // Bone Mask
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 42; // Blue Steel
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 43; // Stained
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 44; // Case Hardened
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 143; // Urban Masked
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 175; // Scorched
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 323; // Rust Coat
									}
									else if (Skin == 15)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 16)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 17)
									{
										*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
									}
									else if (Skin == 18)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 19)
									{
										*pWeapon->FallbackPaintKit() = 414; // Rust Coat
									}
									else if (Skin == 20)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
									}
									else if (Skin == 21)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
									}
									else if (Skin == 22)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
									}
									else if (Skin == 23)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 24)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 25)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 26)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 27)
									{
										*pWeapon->FallbackPaintKit() = 570; // Doppler Phase 4
									}
									else if (Skin == 28)
									{
										*pWeapon->FallbackPaintKit() = 568; // Doppler Phase 4
									}
									else if (Skin == 27)
									{
										*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
									}
									else if (Skin == 28)
									{
										*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
									}
									else if (Skin == 29)
									{
										*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
									}
									else if (Skin == 30)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
									}
									else if (Skin == 31)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
									}
									else if (Skin == 32)
									{
										*pWeapon->FallbackPaintKit() = 561; // Lore
									}

								}
								else if (Model == 8) // M9 Bayonet
								{
									*pWeapon->ModelIndex() = iM9Bayonet; // m_nModelIndex
									*pWeapon->ViewModelIndex() = iM9Bayonet;
									*pWeapon->WorldModelIndex() = iM9Bayonet + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 508;

									int Skin = Menu::Window.SkinTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 27; // Bone Mask
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 42; // Blue Steel
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 43; // Stained
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 44; // Case Hardened
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 143; // Urban Masked
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 175; // Scorched
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 323; // Rust Coat
									}
									else if (Skin == 15)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 16)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 17)
									{
										*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
									}
									else if (Skin == 18)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 19)
									{
										*pWeapon->FallbackPaintKit() = 414; // Rust Coat
									}
									else if (Skin == 20)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
									}
									else if (Skin == 21)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
									}
									else if (Skin == 22)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
									}
									else if (Skin == 23)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 24)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 25)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 26)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 27)
									{
										*pWeapon->FallbackPaintKit() = 570; // Doppler Phase 4
									}
									else if (Skin == 28)
									{
										*pWeapon->FallbackPaintKit() = 568; // Doppler Phase 4
									}
									else if (Skin == 27)
									{
										*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
									}
									else if (Skin == 28)
									{
										*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
									}
									else if (Skin == 29)
									{
										*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
									}
									else if (Skin == 30)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
									}
									else if (Skin == 31)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
									}
									else if (Skin == 32)
									{
										*pWeapon->FallbackPaintKit() = 562; // Lore
									}

								}


								else if (Model == 10) // Shadow Daggers
								{
									*pWeapon->ModelIndex() = iDagger; // m_nModelIndex
									*pWeapon->ViewModelIndex() = iDagger;
									*pWeapon->WorldModelIndex() = iDagger + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 516;

									int Skin = Menu::Window.SkinTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 5; // Forest DDPAT
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 27; // Bone Mask
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 42; // Blue Steel
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 43; // Stained
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 44; // Case Hardened
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 143; // Urban Masked
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 175; // Scorched
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 323; // Rust Coat
									}
									else if (Skin == 15)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 16)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 17)
									{
										*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
									}
									else if (Skin == 18)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 19)
									{
										*pWeapon->FallbackPaintKit() = 414; // Rust Coat
									}
									else if (Skin == 20)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
									}
									else if (Skin == 21)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
									}
									else if (Skin == 22)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
									}
									else if (Skin == 23)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 24)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 25)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 26)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 27)
									{
										*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
									}
									else if (Skin == 28)
									{
										*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
									}
									else if (Skin == 29)
									{
										*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
									}
									else if (Skin == 30)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
									}
									else if (Skin == 31)
									{
										*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
									}
									else if (Skin == 32)
									{
										*pWeapon->FallbackPaintKit() = 561; // Lore
									}
								}
							}

							*pWeapon->OwnerXuidLow() = 0;
							*pWeapon->OwnerXuidHigh() = 0;
							*pWeapon->FallbackWear() = 0.001f;
							*pWeapon->m_AttributeManager()->m_Item()->ItemIDHigh() = 1;

							}
						}
					}
				}
			}
		}
	}
	oFrameStageNotify(curStage);
}

void __fastcall Hooked_OverrideView(void* ecx, void* edx, CViewSetup* pSetup)
{
	IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
	{
		if (Menu::Window.VisualsTab.Active.GetState() && pLocal->IsAlive() && !pLocal->IsScoped())
		{
			if (pSetup->fov = 90)
				pSetup->fov = Menu::Window.VisualsTab.OtherFOV.GetValue();
		}

		oOverrideView(ecx, edx, pSetup);
	}

}

void GetViewModelFOV(float& fov)
{
	IClientEntity* localplayer = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
	{

		if (!localplayer)
			return;


		if (Menu::Window.VisualsTab.Active.GetState())
		fov += Menu::Window.VisualsTab.OtherViewmodelFOV.GetValue();
	}
}

float __stdcall GGetViewModelFOV()
{
	float fov = Hooks::VMTClientMode.GetMethod<oGetViewModelFOV>(35)();

	GetViewModelFOV(fov);

	return fov;
}

void __fastcall Hooked_RenderView(void* ecx, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw)
{
	static DWORD oRenderView = Hooks::VMTRenderView.GetOriginalFunction(6);

	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	__asm
	{
		PUSH whatToDraw
		PUSH nClearFlags
		PUSH hudViewSetup
		PUSH setup
		MOV ECX, ecx
		CALL oRenderView
	}
}