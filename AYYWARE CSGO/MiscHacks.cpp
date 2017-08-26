#define _CRT_SECURE_NO_WARNINGS

#include "MiscHacks.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "Menu.h"

#include <time.h>

template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}

inline float bitsToFloat(unsigned long i)
{
	return *reinterpret_cast<float*>(&i);
}

inline float FloatNegate(float f)
{
	return bitsToFloat(FloatBits(f) ^ 0x80000000);
}

Vector AutoStrafeView;

void CMiscHacks::Init()
{
	// Any init
}
#define charenc( s ) ( s )

void yanderename() {
	static bool steal = false;

	ConVar* nameConvar = Interfaces::CVar->FindVar(charenc("name"));
	*(int*)((DWORD)&nameConvar->fnChangeCallback + 0xC) = 0;

	if (nameConvar)
	{
		if (steal)
		{
			nameConvar->SetValue("SUICIDE.CC");
		}
		else
		{
			nameConvar->SetValue("SUICIDE.CC");
		}
		steal = !steal;
	}
}
void yanderecolname() {
	static bool steal = false;

	ConVar* nameConvar = Interfaces::CVar->FindVar(charenc("name"));
	*(int*)((DWORD)&nameConvar->fnChangeCallback + 0xC) = 0;

	if (nameConvar)
	{
		if (steal)
		{
			nameConvar->SetValue("\x01\x0B\x0D""S""\x0D""U""\x0D""I""\x0D""C""\x0D""I""\x0D""D""\x0D""E""\x0D"".""\x0D""C""\x0D""C""\x0D");
		}
		else
		{
			nameConvar->SetValue("\x01\x0B\x03""S""\x03""U""\x03""I""\x03""C""\x03""I""\x03""D""\x03""E""\x03"".""\x03""C""\x03""C""\x03");
		}
		steal = !steal;
	}
}
void blankname() {

	ConVar* nameConvar = Interfaces::CVar->FindVar(charenc("name"));
	*(int*)((DWORD)&nameConvar->fnChangeCallback + 0xC) = 0;

	nameConvar->SetValue("\n");
}
void puname() {
	static bool steal = false;

	ConVar* nameConvar = Interfaces::CVar->FindVar(charenc("name"));
	*(int*)((DWORD)&nameConvar->fnChangeCallback + 0xC) = 0;

	if (nameConvar)
	{
		if (steal)
		{
			nameConvar->SetValue("\n PU ROCKS! \n");
		}
		else
		{
			nameConvar->SetValue("\n PU ROCKS! \n");
		}
		steal = !steal;
	}
}


void awname() {
	static bool steal = false;

	ConVar* nameConvar = Interfaces::CVar->FindVar(charenc("name"));
	*(int*)((DWORD)&nameConvar->fnChangeCallback + 0xC) = 0;

	if (nameConvar)
	{
		if (steal)
		{
			nameConvar->SetValue("™AIMWARE.net");
		}
		else
		{
			nameConvar->SetValue("™AIMWARE.net ");
		}
		steal = !steal;
	}
}

void iwebzname() {
	static bool steal = false;

	ConVar* nameConvar = Interfaces::CVar->FindVar(charenc("name"));
	*(int*)((DWORD)&nameConvar->fnChangeCallback + 0xC) = 0;

	if (nameConvar)
	{
		if (steal)
		{
			nameConvar->SetValue("INTERWEBZ-");
		}
		else
		{
			nameConvar->SetValue("-INTERWEBZ");
		}
		steal = !steal;
	}
}

void CMiscHacks::Draw()
{
	// Any drawing	
	// Spams

}

void CMiscHacks::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	// Any Move Stuff
	
	// Bhop
	switch (Menu::Window.MiscTab.OtherAutoJump.GetIndex())
	{
	case 0:
		break;
	case 1:
		AutoJump(pCmd);
		break;
	}

	switch (Menu::Window.MiscTab.OtherChatSpam.GetIndex())
	{
	case 0:
		// No Chat Spam
		break;
		/*case 1:
		// Namestealer
		ChatSpamName();
		break;*/
	case 1:
		// Regular
		ChatSpamRegular();
		break;
	}

	// AutoStrafe
	Interfaces::Engine->GetViewAngles(AutoStrafeView);
	switch (Menu::Window.MiscTab.OtherAutoStrafe.GetIndex())
	{
	case 0:
		// Off
		break;
	case 1:
		LegitStrafe(pCmd);
		break;
	case 2:
		RageStrafe(pCmd);
		break;
	}
	runname();
	//Fake Lag
	if (Menu::Window.MiscTab.FakeLagEnable.GetState())
		Fakelag(pCmd, bSendPacket);
	//FakeWalk
	if (Menu::Window.RageBotTab.AntiAimFakeWalk.GetState())
		FakeWalk(pCmd, bSendPacket);
	if (Menu::Window.MiscTab.MoonWalk.GetState())
		MoonWalk(pCmd);
}
void CMiscHacks::runname()
{
	switch (Menu::Window.MiscTab.OtherNameSpam.GetIndex())
	{
	case 0: break;
	case 1: yanderename(); break;
	case 2: yanderecolname(); break;
	case 3: blankname(); break;
	case 4: puname(); break;
	case 5: awname(); break;
	case 6: iwebzname(); break;

		//case 9: NameSteal(); break;
	}
}
static __declspec(naked) void __cdecl Invoke_NET_SetConVar(void* pfn, const char* cvar, const char* value)
{
	__asm 
	{
		push    ebp
			mov     ebp, esp
			and     esp, 0FFFFFFF8h
			sub     esp, 44h
			push    ebx
			push    esi
			push    edi
			mov     edi, cvar
			mov     esi, value
			jmp     pfn
	}
}

void DECLSPEC_NOINLINE NET_SetConVar(const char* value, const char* cvar)
{
	static DWORD setaddr = Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x8D\x4C\x24\x1C\xE8\x00\x00\x00\x00\x56", "xxxxx????x");
	if (setaddr != 0) 
	{
		void* pvSetConVar = (char*)setaddr;
		Invoke_NET_SetConVar(pvSetConVar, cvar, value);
	}
}

void change_name(const char* name)
{
	if (Interfaces::Engine->IsInGame() && Interfaces::Engine->IsConnected())
		NET_SetConVar(name, "name");
}

void CMiscHacks::AutoJump(CUserCmd *pCmd)
{
	if (pCmd->buttons & IN_JUMP && GUI.GetKeyState(VK_SPACE))
	{
		int iFlags = hackManager.pLocal()->GetFlags();
		if (!(iFlags & FL_ONGROUND))
			pCmd->buttons &= ~IN_JUMP;

		if (hackManager.pLocal()->GetVelocity().Length() <= 50)
		{
			pCmd->forwardmove = 450.f;
		}
	}
}
void CMiscHacks::MoonWalk(CUserCmd *pCmd)
{
	if (pCmd->forwardmove > 0)
	{
		pCmd->buttons |= IN_BACK;
		pCmd->buttons &= ~IN_FORWARD;
	}

	if (pCmd->forwardmove < 0)
	{
		pCmd->buttons |= IN_FORWARD;
		pCmd->buttons &= ~IN_BACK;
	}

	if (pCmd->sidemove < 0)
	{
		pCmd->buttons |= IN_MOVERIGHT;
		pCmd->buttons &= ~IN_MOVELEFT;
	}

	if (pCmd->sidemove > 0)
	{
		pCmd->buttons |= IN_MOVELEFT;
		pCmd->buttons &= ~IN_MOVERIGHT;
	}
}
void CMiscHacks::FakeWalk(CUserCmd* pCmd, bool & bSendPacket)
{
	IClientEntity* pLocal = hackManager.pLocal();
	if (GetAsyncKeyState(VK_SHIFT))
	{
		static int iChoked = -1;
		iChoked++;

		if (iChoked < 1)
		{
			bSendPacket = false;

			pCmd->tick_count += 10;
			pCmd->command_number += 7 + pCmd->tick_count % 2 ? 0 : 1;

			pCmd->buttons |= pLocal->GetMoveType() == IN_BACK;
			pCmd->forwardmove = pCmd->sidemove = 0.f;
		}
		else
		{
			bSendPacket = true;
			iChoked = -1;

			Interfaces::Globals->frametime *= (pLocal->GetVelocity().Length2D()) / 1.f;
			pCmd->buttons |= pLocal->GetMoveType() == IN_FORWARD;
		}
	}
}

void CMiscHacks::LegitStrafe(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();
	if (!(pLocal->GetFlags() & FL_ONGROUND))
	{
		pCmd->forwardmove = 0.0f;

		if (pCmd->mousedx < 0)
		{
			pCmd->sidemove = -450.0f;
		}
		else if (pCmd->mousedx > 0)
		{
			pCmd->sidemove = 450.0f;
		}
	}
}

void CMiscHacks::RageStrafe(CUserCmd *pCmd)
{
	float strafespeed = Menu::Window.MiscTab.OtherStrafeSpeed.GetValue();

	IClientEntity* pLocal = hackManager.pLocal();

	bool bKeysPressed = true;
	if (GUI.GetKeyState(0x41) || GUI.GetKeyState(0x57) || GUI.GetKeyState(0x53) || GUI.GetKeyState(0x44)) bKeysPressed = false;

	if ((GetAsyncKeyState(VK_SPACE) && !(pLocal->GetFlags() & FL_ONGROUND)) && bKeysPressed)
	{
		pCmd->forwardmove = (1550.f * strafespeed) / pLocal->GetVelocity().Length2D();
		pCmd->sidemove = (pCmd->command_number % 2) == 0 ? -450.f : 450.f;
		if (pCmd->forwardmove > 450.f)
			pCmd->forwardmove = 450.f;
	}
}

Vector GetAutostrafeView()
{
	return AutoStrafeView;
}

void CMiscHacks::ChatSpamInterwebz()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	static bool wasSpamming = true;
	//static std::string nameBackup = "INTERWEBZ";

	if (wasSpamming)
	{
		static bool useSpace = true;
		if (useSpace)
		{
			change_name ("INTERWEBZ-");
			useSpace = !useSpace;
		}
		else
		{
			change_name("-INTERWEBZ");
			useSpace = !useSpace;
		}
	}

	start_t = clock();
}

void CMiscHacks::ChatSpamDisperseName()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	static bool wasSpamming = true;

	if (wasSpamming)
	{
		change_name("\n…e…e…e\n");
	}

	start_t = clock();
}

void CMiscHacks::ChatSpamName()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	std::vector < std::string > Names;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *entity = Interfaces::EntList->GetClientEntity(i);

		player_info_t pInfo;
		// If it's a valid entity and isn't the player
		if (entity && hackManager.pLocal()->GetTeamNum() == entity->GetTeamNum() && entity != hackManager.pLocal())
		{
			ClientClass* cClass = (ClientClass*)entity->GetClientClass();

			// If entity is a player
			if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
			{
				if (Interfaces::Engine->GetPlayerInfo(i, &pInfo))
				{
					if (!strstr(pInfo.name, "GOTV"))
						Names.push_back(pInfo.name);
				}
			}
		}
	}

	static bool wasSpamming = true;
	//static std::string nameBackup = "INTERWEBZ.CC";

	int randomIndex = rand() % Names.size();
	char buffer[128];
	sprintf_s(buffer, "%s ", Names[randomIndex].c_str());

	if (wasSpamming)
	{
		change_name(buffer);
	}
	else
	{
		change_name ("p$i 1337");
	}

	start_t = clock();
}


void CMiscHacks::ChatSpamRegular()
{
	// Don't spam it too fast so you can still do stuff
	static clock_t start_t = clock();
	int spamtime = Menu::Window.MiscTab.OtherChatDelay.GetValue();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < spamtime)
		return;

	static bool holzed = true;

	if (Menu::Window.MiscTab.OtherTeamChat.GetState())
	{
		SayInTeamChat("SUICIDE.CC OWNS ME AND ALL");
	}
	else
	{
		SayInChat("SUICIDE.CC OWNS ME AND ALL");
	}

	start_t = clock();
}

void CMiscHacks::Fakelag(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pLocal = hackManager.pLocal();
	if (pCmd->buttons & IN_ATTACK) return;
	if (pLocal->GetVelocity() == Vector(0, 0, 0)) return;

	int iChoke = Menu::Window.MiscTab.FakeLagChoke.GetValue();

	static int iFakeLag = -1;
	iFakeLag++;

	if (iFakeLag <= iChoke && iFakeLag > -1)
	{
		bSendPacket = false;
	}
	else
	{
		bSendPacket = true;
		iFakeLag = -1;
	}
}
