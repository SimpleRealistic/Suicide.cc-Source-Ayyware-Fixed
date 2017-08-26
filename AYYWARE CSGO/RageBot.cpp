#include "RageBot.h"
#include "RenderManager.h"
#include "Autowall.h"
#include <iostream>
#include "UTIL Functions.h"
#include "AntiAntiAim.h"
#include "Nospread.h"

#include <stdlib.h>
#include <intrin.h>
#include <math.h>

#define TICK_INTERVAL			( Interfaces::Globals->interval_per_tick )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )





template< class T, class Y >
T ClampZZZZ(T const &val, Y const &minVal, Y const &maxVal)
{
	if (val < minVal)
		return minVal;
	else if (val > maxVal)
		return maxVal;
	else
		return val;
}

float LagFix()
{
	float updaterate = Interfaces::CVar->FindVar("cl_updaterate")->fValue;
	ConVar* minupdate = Interfaces::CVar->FindVar("sv_minupdaterate");
	ConVar* maxupdate = Interfaces::CVar->FindVar("sv_maxupdaterate");

	if (minupdate && maxupdate)
		updaterate = maxupdate->fValue;

	float ratio = Interfaces::CVar->FindVar("cl_interp_ratio")->fValue;

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = Interfaces::CVar->FindVar("cl_interp")->fValue;
	ConVar* cmin = Interfaces::CVar->FindVar("sv_client_min_interp_ratio");
	ConVar* cmax = Interfaces::CVar->FindVar("sv_client_max_interp_ratio");

	if (cmin && cmax && cmin->fValue != 1)
		ratio = ClampZZZZ(ratio, cmin->fValue, cmax->fValue);

	return max(lerp, ratio / updaterate);
}

void CRageBot::StartLagCompensation(IClientEntity* pEntity, CUserCmd* pCmd)
{
	float flSimTime = pEntity->GetSimulationTime();
	pCmd->tick_count = TIME_TO_TICKS(flSimTime + 0.031f);
}

void CRageBot::Init()
{
	IsAimStepping = false;
	IsLocked = false;
	TargetID = -1;
}

void CRageBot::Draw()
{

}

bool IsAbleToShoot(IClientEntity* pLocal)
{
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!pLocal)
		return false;

	if (!pWeapon)
		return false;

	float flServerTime = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;

	return (!(pWeapon->GetNextPrimaryAttack() > flServerTime));
}

// The Interp Fix
float InterpolationFix()
{
	static ConVar* cvar_cl_interp = Interfaces::CVar->FindVar("cl_interp");
	static ConVar* cvar_cl_updaterate = Interfaces::CVar->FindVar("cl_updaterate");
	static ConVar* cvar_sv_maxupdaterate = Interfaces::CVar->FindVar("sv_maxupdaterate");
	static ConVar* cvar_sv_minupdaterate = Interfaces::CVar->FindVar("sv_minupdaterate");
	static ConVar* cvar_cl_interp_ratio = Interfaces::CVar->FindVar("cl_interp_ratio");

	IClientEntity* pLocal = hackManager.pLocal();
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

	float cl_interp = cvar_cl_interp->GetFloat();
	int cl_updaterate = cvar_cl_updaterate->GetInt();
	int sv_maxupdaterate = cvar_sv_maxupdaterate->GetInt();
	int sv_minupdaterate = cvar_sv_minupdaterate->GetInt();
	int cl_interp_ratio = cvar_cl_interp_ratio->GetInt();

	if (sv_maxupdaterate <= cl_updaterate)
		cl_updaterate = sv_maxupdaterate;

	if (sv_minupdaterate > cl_updaterate)
		cl_updaterate = sv_minupdaterate;

	float new_interp = (float)cl_interp_ratio / (float)cl_updaterate;

	if (new_interp > cl_interp)
		cl_interp = new_interp;

	return max(cl_interp, cl_interp_ratio / cl_updaterate);
}

float RandomFloat(float min, float max)
{
	typedef float(*RandomFloat_t)(float, float);
	static RandomFloat_t m_RandomFloat = (RandomFloat_t)GetProcAddress(GetModuleHandle(TEXT("vstdlib.dll")), "RandomFloat");
	return m_RandomFloat(min, max);
}

float hitchance(IClientEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	float hitchance = 101;
	float hitmeme = Menu::Window.RageBotTab.AccuracyHitchance.GetValue();
	int chance;
	QAngle qAngle;

	if (!chance)
		return true;

	int iHit = 0;
	int iHitsNeed = (int)(hitmeme * ((float)chance / 100.f));

	if (Menu::Window.RageBotTab.AccuracyHitchance.GetValue() > 1)
	{
		float inaccuracy = pWeapon->GetInaccuracy();
		if (inaccuracy == 0)
		{
			inaccuracy = 0.0000001;
		}
		if ((int)(((float)iHit / hitmeme) * 100.f) >= chance)
		{
			inaccuracy = 1 / inaccuracy;
			hitchance = inaccuracy;
		}
	}
	return hitchance;
}

bool CanOpenFire()
{
	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalEntity)
		return false;

	CBaseCombatWeapon* entwep = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocalEntity->GetActiveWeaponHandle());

	float flServerTime = (float)pLocalEntity->GetTickBase() * Interfaces::Globals->interval_per_tick;
	float flNextPrimaryAttack = entwep->GetNextPrimaryAttack();

	std::cout << flServerTime << " " << flNextPrimaryAttack << std::endl;

	return !(flNextPrimaryAttack > flServerTime);
}

void CRageBot::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalEntity)
		return;

	// Master switch
	if (!Menu::Window.RageBotTab.Active.GetState())
		return;

	// Anti Aim 
	if (Menu::Window.RageBotTab.AntiAimEnable.GetState())
	{
		static int ChokedPackets = -1;

		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
		if (!pWeapon)
			return;

		if (ChokedPackets < 1 && pLocalEntity->GetLifeState() == LIFE_ALIVE && pCmd->buttons & IN_ATTACK && CanOpenFire() && GameUtils::IsBallisticWeapon(pWeapon))
		{
			bSendPacket = false;
		}
		else
		{
			if (pLocalEntity->GetLifeState() == LIFE_ALIVE)
			{
				DoAntiAim(pCmd, bSendPacket);
			}
			ChokedPackets = -1;
		}
	}

	// Aimbot
	if (Menu::Window.RageBotTab.AimbotEnable.GetState())
		DoAimbot(pCmd, bSendPacket);

	// Recoil
	if (Menu::Window.RageBotTab.AccuracyRecoil.GetState())
		DoNoRecoil(pCmd);

	// Aimstep
	if (Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = pCmd->viewangles - LastAngle;
		if (AddAngs.Length2D() > 25.f)
		{
			Normalize(AddAngs, AddAngs);
			AddAngs *= 25;
			pCmd->viewangles = LastAngle + AddAngs;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}

	LastAngle = pCmd->viewangles;
}

Vector TickPrediction(Vector AimPoint, IClientEntity* pTarget)
{
	return AimPoint + (pTarget->GetVelocity() * Interfaces::Globals->interval_per_tick);
}

Vector BestPoint(IClientEntity *targetPlayer, Vector &final)
{
	IClientEntity* pLocal = hackManager.pLocal();

	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = targetPlayer;
	ray.Init(final + Vector(0, 0, 10), final);
	Interfaces::Trace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	final = tr.endpos;
	return final;

}

// Functionality
void CRageBot::DoAimbot(CUserCmd *pCmd,bool &bSendPacket) // Ragebot, Aimbot
{
	IClientEntity* pTarget = nullptr;
	IClientEntity* pLocal = hackManager.pLocal();
	Vector Start = pLocal->GetViewOffset() + pLocal->GetOrigin();
	IClientEntity *pEntity = nullptr;
	bool FindNewTarget = true;
	//IsLocked = false;

	CSWeaponInfo* weapInfo = ((CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle()))->GetCSWpnData();

	// Don't aimbot with the knife..
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	// Auto Revolver
	if (GameUtils::IsRevolver(pWeapon) && Menu::Window.RageBotTab.AimbotAutoRevolver.GetState())
	{
		static int delay = 0;
		delay++;

		if (delay <= 15)
			pCmd->buttons |= IN_ATTACK;
		else
			delay = 0;
	}

	// Aimbot won't run if it's a knife or grenades
	if (pWeapon)
	{
		if (pWeapon->GetAmmoInClip() == 0 || !GameUtils::IsBallisticWeapon(pWeapon))
		{
			//TargetID = 0;
			//pTarget = nullptr;
			//HitBox = -1;
			return;
		}
	}
	else
		return;

	if (IsLocked && TargetID >= 0 && HitBox >= 0)
	{
		pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		if (pTarget  && TargetMeetsRequirements(pTarget))
		{
			HitBox = HitScan(pTarget);
			if (HitBox >= 0)
			{
				Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
				Vector View;
				Interfaces::Engine->GetViewAngles(View);

				float FoV = FovToPlayer(ViewOffset, View, pTarget, HitBox);
				if (FoV < Menu::Window.RageBotTab.AimbotFov.GetValue())
					FindNewTarget = false;
			}
		}
	}

	// Find a new target, apparently we need to
	if (FindNewTarget)
	{
		TargetID = 0;
		pTarget = nullptr;
		HitBox = -1;

		// Target selection type
		switch (Menu::Window.RageBotTab.TargetSelection.GetIndex())
		{
		case 0:
			TargetID = GetTargetCrosshair();
			break;
		case 1:
			TargetID = GetTargetDistance();
			break;
		case 2:
			TargetID = GetTargetHealth();
			break;
		}

		// Memes
		if (TargetID >= 0)
		{
			pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		}
		else
		{
			pTarget = nullptr;
			HitBox = -1;
		}
	} 

	Globals::Target = pTarget;
	Globals::TargetID = TargetID;

	// If we finally have a good target
	if (TargetID >= 0 && pTarget)
	{
		// Get the hitbox to shoot at
		HitBox = HitScan(pTarget);

		if (!CanOpenFire())
			return;

		// Key
		if (Menu::Window.RageBotTab.AimbotKeyPress.GetState())
		{
			int Key = Menu::Window.RageBotTab.AimbotKeyBind.GetKey();
			if (Key >= 0 && !GUI.GetKeyState(Key))
			{
				TargetID = -1;
				pTarget = nullptr;
				HitBox = -1;
				return;
			}
		}

		// Stop key
		int StopKey = Menu::Window.RageBotTab.AimbotStopKey.GetKey();
		if (StopKey >= 0 && GUI.GetKeyState(StopKey))
		{
			TargetID = -1;
			pTarget = nullptr;
			HitBox = -1;
			return;
		}

		Vector Point;
		float pointscale = Menu::Window.RageBotTab.TargetPointscale.GetValue();
		Vector AimPoint = GetHitboxPosition(pTarget, HitBox) + Vector(0, 0, (pointscale / 100));

		if (Menu::Window.RageBotTab.TargetMultipoint.GetState())
		{
			Point = BestPoint(pTarget, AimPoint);
		}
		else
		{
			Point = AimPoint;
		}

		if (Menu::Window.RageBotTab.PerfectAccuracy.GetState())
			pCmd->tick_count = TIME_TO_TICKS(InterpolationFix());

		if (Menu::Window.RageBotTab.FakeLagFix.GetState())
			pCmd->tick_count = TIME_TO_TICKS(LagFix());




		if (Menu::Window.RageBotTab.PerfectAccuracy.GetState())
			Point = TickPrediction(Point, pTarget);

		if (Menu::Window.RageBotTab.PerfectAccuracy.GetState())
			StartLagCompensation(pLocal, pCmd);

		// Velocity Prediction
		pTarget->GetPredicted(AimPoint);

		if (GameUtils::IsScopedWeapon(pWeapon) && !pWeapon->IsScoped() && Menu::Window.RageBotTab.AccuracyAutoScope.GetState()) // Autoscope
		{
			pCmd->buttons |= IN_ATTACK2;
		}
		else
		{
			if ((Menu::Window.RageBotTab.AccuracyHitchance.GetValue() * 1.5 <= hitchance(pLocal, pWeapon)) || Menu::Window.RageBotTab.AccuracyHitchance.GetValue() == 0 || *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 64)
			{
				if (AimAtPoint(pLocal, Point, pCmd, bSendPacket))
				{
					if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
					{
						pCmd->buttons |= IN_ATTACK;
					}
					else
					{
						return;
					}
				}
				else if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
				{
					pCmd->buttons |= IN_ATTACK;
				}
			}
		}

		if (IsAbleToShoot(pLocal) && pCmd->buttons & IN_ATTACK)
			Globals::Shots += 1;

		// Stop and Crouch
		if (TargetID >= 0 && pTarget)
		{
			if (Menu::Window.RageBotTab.AccuracyAutoStop.GetState())
			{
				pCmd->forwardmove = 0.f;
				pCmd->sidemove = 0.f;
				pCmd->buttons |= IN_DUCK;
			}
		}
	}

	// Auto Pistol
	if (GameUtils::IsPistol(pWeapon) && Menu::Window.RageBotTab.AimbotAutoPistol.GetState())
	{
		if (pCmd->buttons & IN_ATTACK)
		{
			static bool WasFiring = false;
			WasFiring = !WasFiring;
			
			if (WasFiring)
			{
				pCmd->buttons |= IN_ATTACK2;
			}
		}
	}
}

bool CRageBot::TargetMeetsRequirements(IClientEntity* pEntity)
{
	// Is a valid player
	if (pEntity && pEntity->IsDormant() == false && pEntity->IsAlive() && pEntity->GetIndex() != hackManager.pLocal()->GetIndex())
	{
		// Entity Type checks
		ClientClass *pClientClass = pEntity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)CSGOClassID::CCSPlayer && Interfaces::Engine->GetPlayerInfo(pEntity->GetIndex(), &pinfo))
		{
			// Team Check
			if (pEntity->GetTeamNum() != hackManager.pLocal()->GetTeamNum() || Menu::Window.RageBotTab.TargetFriendlyFire.GetState())
			{
				// Spawn Check
				if (!pEntity->HasGunGameImmunity())
				{
					return true;
				}
			}
		}
	}

	// They must have failed a requirement
	return false;
}

float CRageBot::FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int aHitBox)
{
	CONST FLOAT MaxDegrees = 180.0f;
	Vector Angles = View;
	Vector Origin = ViewOffSet;
	Vector Delta(0, 0, 0);
	Vector Forward(0, 0, 0);
	AngleVectors(Angles, &Forward);
	Vector AimPos = GetHitboxPosition(pEntity, aHitBox);
	VectorSubtract(AimPos, Origin, Delta);
	Normalize(Delta, Delta);
	FLOAT DotProduct = Forward.Dot(Delta);
	return (acos(DotProduct) * (MaxDegrees / PI));
}

int CRageBot::GetTargetCrosshair()
{
	// Target selection
	int target = -1;
	float minFoV = Menu::Window.RageBotTab.AimbotFov.GetValue();

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++) //GetHighestEntityIndex()
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (fov < minFoV)
				{
					minFoV = fov;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::GetTargetDistance()
{
	// New Distance Aimbot
	int target = -1;
	int minDist = 8192;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				Vector Difference = pLocal->GetOrigin() - pEntity->GetOrigin();
				int Distance = Difference.Length();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Distance < minDist && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minDist = Distance;
					target = i;
				}
			}
		}
	}
	return target;

	/* 
	// Target selection
	int target = -1;
	int minDist = 9999999999;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				Vector Difference = pLocal->GetOrigin() - pEntity->GetOrigin();
				int Distance = Difference.Length();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Distance < minDist && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minDist = Distance;
					target = i;
				}
			}
		}
	}

	return target;
	*/
}

int CRageBot::GetTargetHealth()
{
	int target = -1;
	int minHealth = 101;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				int Health = pEntity->GetHealth();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Health < minHealth && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minHealth = Health;
					target = i;
				}
			}
		}
	}
	return target;

	/*
	// Target selection
	int target = -1;
	int minHealth = 101;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				int Health = pEntity->GetHealth();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Health < minHealth && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minHealth = Health;
					target = i;
				}
			}
		}
	}

	return target;
	*/
}

int CRageBot::HitScan(IClientEntity* pEntity)
{
	IClientEntity* pLocal = hackManager.pLocal();
	std::vector<int> HitBoxesToScan;
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

	// Get the hitboxes to scan
	int HitScanMode = Menu::Window.RageBotTab.TargetHitscan.GetIndex();
	int iSmart = Menu::Window.RageBotTab.AccuracySmart.GetValue();
	bool AWall = Menu::Window.RageBotTab.AccuracyAutoWall.GetState();
	bool Multipoint = Menu::Window.RageBotTab.TargetMultipoint.GetState();

	if (iSmart > 0 && pLocal->GetShotsFired() + 1 > iSmart)
	{
		HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
		HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
		HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftHand);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightHand);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
	}
	else
	{
		if (HitScanMode == 0)
		{
			// No Hitscan, just a single hitbox
			switch (Menu::Window.RageBotTab.TargetHitbox.GetIndex())
			{
			case 0:
				HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
				break;
			case 1:
				HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
				break;
			case 2:
				HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
				break;
			case 3:
				HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				break;
			case 4:
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
				break;
			}
		}
		else
		{
			switch (HitScanMode)
			{
			case 1:
				// head/body
				if (Menu::Window.RageBotTab.PreferBodyAim.GetIndex() == 1)
				{

					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				}
				else if (Menu::Window.RageBotTab.PreferBodyAim.GetIndex() == 2)
				{
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				}
				else if (Menu::Window.RageBotTab.AutoAwpBody.GetState() && GameUtils::IsSniper(pWeapon))
				{
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				}
				else
				{
					HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
					HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				}
				break;
			case 2:
				if (Menu::Window.RageBotTab.PreferBodyAim.GetIndex() == 1)
				{

					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				}
				else if (Menu::Window.RageBotTab.PreferBodyAim.GetIndex() == 2)
				{
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				}
				else if (Menu::Window.RageBotTab.AutoAwpBody.GetState() && GameUtils::IsSniper(pWeapon))
				{
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				}
				else
				{
					// basic +(arms, thighs)
					HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
					HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
					HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
					HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
					HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
					HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
				}
				break;
			case 3:
				if (Menu::Window.RageBotTab.PreferBodyAim.GetIndex() == 1)
				{

					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				}
				else if (Menu::Window.RageBotTab.PreferBodyAim.GetIndex() == 2)
				{
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				}
				else if (Menu::Window.RageBotTab.AutoAwpBody.GetState() && GameUtils::IsSniper(pWeapon))
				{
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				}
				else
				{
					// heaps ++(just all the random shit)
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				}
				break;
			case 4:
				if (Menu::Window.RageBotTab.PreferBodyAim.GetIndex() == 1)
				{

					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				}
				else if (Menu::Window.RageBotTab.PreferBodyAim.GetIndex() == 2)
				{
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				}
				else
				{
					//pHITSCANE!!
					HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
					HitBoxesToScan.push_back((int)CSGOHitboxID::NeckLower);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
					HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
					HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
					HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
					HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
					HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
					HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
					HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
					HitBoxesToScan.push_back((int)CSGOHitboxID::RightHand);
					HitBoxesToScan.push_back((int)CSGOHitboxID::LeftHand);
					HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
					HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
					HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
					HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
				}
				break;
			}
		}
	}

#pragma endregion Get the list of shit to scan

	// check hits
	// check hits
	for (auto HitBoxID : HitBoxesToScan)
	{
		if (AWall)
		{
			Vector Point = GetHitboxPosition(pEntity, HitBoxID);
			float Damage = 0.f;
			Color c = Color(255, 255, 255, 255);
			if (CanHit(Point, &Damage))
			{
				c = Color(0, 255, 0, 255);
				if (Damage >= Menu::Window.RageBotTab.AccuracyMinimumDamage.GetValue())
				{
					return HitBoxID;
				}
			}
		}
		else
		{
			if (GameUtils::IsVisible(hackManager.pLocal(), pEntity, HitBoxID))
				return HitBoxID;
		}
	}

	return -1;
}

void CRageBot::DoNoRecoil(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();
	if (pLocal)
	{
		Vector AimPunch = pLocal->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			pCmd->viewangles -= AimPunch * 2;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}
}

bool CRageBot::AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket)
{
	bool ReturnValue = false;
	if (point.Length() == 0) return ReturnValue;
	Vector angles;
	Vector src = pLocal->GetOrigin() + pLocal->GetViewOffset();
	CalcAngle(src, point, angles);
	GameUtils::NormaliseViewAngle(angles);
	IsLocked = true;

	// Aim Step Calcs
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	if (!IsAimStepping)
		LastAimstepAngle = LastAngle; // Don't just use the viewangs because you need to consider aa

	float fovLeft = FovToPlayer(ViewOffset, LastAimstepAngle, Interfaces::EntList->GetClientEntity(TargetID), 0);

	if (fovLeft > 25.0f && Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = angles - LastAimstepAngle;
		Normalize(AddAngs, AddAngs);
		AddAngs *= 25;
		LastAimstepAngle += AddAngs;
		GameUtils::NormaliseViewAngle(LastAimstepAngle);
		angles = LastAimstepAngle;
	}
	else
	{
		ReturnValue = true;
	}


	Vector Oldview = pCmd->viewangles;

	switch (Menu::Window.RageBotTab.AimbotSlientSelection.GetIndex()) {

	case 0:
		// Disabled
		Interfaces::Engine->SetViewAngles(angles);
		break;

	case 1:
		// Client Side
		pCmd->viewangles = angles;
		break;

	case 2:
		// Server Side
		static int ChokedPackets = -1;
		ChokedPackets++;

		if (ChokedPackets < 6)
		{
			bSendPacket = false;
			pCmd->viewangles = angles;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles = Oldview;
			ChokedPackets = -1;
			ReturnValue = false;
		}
		break;
	}
	return ReturnValue;
}

namespace AntiAims
{
	static bool flip = false;
	// Pitches
	void StaticPitch(CUserCmd *pCmd)
	{
		// Up
		pCmd->viewangles.x = 89.f;
	}

	void JitterPitch(CUserCmd *pCmd)
	{
		static bool up = true;
		if (up) pCmd->viewangles.x = -89.f;
		else pCmd->viewangles.x = 89.f;
		up = !up;
	}

	void FakePitch(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChockedPackets = -1;
		ChockedPackets++;
		if (ChockedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.x = 89.999f;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.x = 51.f;
			ChockedPackets = -1;
		}
	}
	void Backward(CUserCmd* pCmd)
	{
		pCmd->viewangles.y -= 180.00f;
	}
	void FakeBackward(CUserCmd* pCmd, bool& bSendPacket)
	{
		if (bSendPacket)
		{
			pCmd->viewangles.y += 180;
		}
		else
		{
			pCmd->viewangles.y -= 90;
		}
	}
	void Emotion2(CUserCmd *pCmd, bool& bSendPacket) {
		pCmd->viewangles.x = -89.f;
		if (bSendPacket)
			pCmd->viewangles.x = 89.f;
	}
	void clickbait(CUserCmd *pCmd)
	{
		{
			int random = rand() % 100;
			int random2 = rand() % 1000;

			static bool dir;
			static float current_x = pCmd->viewangles.x;

			if (random == 1) dir = !dir;

			if (dir)
				current_x += 700;
			else
				current_x -= 34;

			pCmd->viewangles.x = current_x;

			if (random == random2)
				pCmd->viewangles.x += random;

		}

	}
	void LBYJitter(CUserCmd* pCmd, bool& bSendPacket)
	{
		static bool ySwitch;
		static bool jbool;
		static bool jboolt;
		ySwitch = !ySwitch;
		jbool = !jbool;
		jboolt = !jbool;

		if (ySwitch)
		{
			if (jbool)
			{
				if (jboolt)
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 90.f;
					bSendPacket = false;
				}
				else
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 90.f;
					bSendPacket = false;
				}
			}
			else
			{
				if (jboolt)
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 125.f;
					bSendPacket = false;
				}
				else
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 125.f;
					bSendPacket = false;
				}
			}
		}
		else
		{
			pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw();
			bSendPacket = true;
		}
	}
	void FakeLowerBody135(CUserCmd *pCmd, bool &bSendPacket)
	{

		int flip = (int)floorf(Interfaces::Globals->curtime / 1.1) % 2;

		static bool bFlipYaw;
		float flInterval = Interfaces::Globals->interval_per_tick;
		float flTickcount = pCmd->tick_count;
		float flTime = flInterval * flTickcount;
		if (std::fmod(flTime, 1) == 0.f)
			bFlipYaw = !bFlipYaw;

		if (bSendPacket)
		{
			if (flip)
			{
				pCmd->viewangles.y += bFlipYaw ? 135.f : -135.f;

			}
			else
			{
				pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + bFlipYaw ? 135.f : -135.f;
			}
		}
		else
		{
			pCmd->viewangles.y += 180.f;
		}
	}
	// Yaws
	void FakeSideways(CUserCmd* pCmd, bool& bSendPacket)
	{

		static bool flip;
		static bool flip2;
		flip = !flip;
		flip2 = !flip2;

		if (pCmd->command_number % 2)
		{
			bSendPacket = false;

			if (flip2)
				pCmd->viewangles.y += 90.5f;

			else
				pCmd->viewangles.y -= 90.5f;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y += 180.0f;
		}
	}
	void FakeTwoStep(CUserCmd* pCmd, bool& bSendPacket)
	{

		static bool bFlipYaw;
		float flInterval = Interfaces::Globals->interval_per_tick;
		float flTickcount = pCmd->tick_count;
		float flTime = flInterval * flTickcount;
		if (std::fmod(flTime, 1) == 0.f)
			bFlipYaw = !bFlipYaw;

		if (bSendPacket)
			pCmd->viewangles.y += bFlipYaw ? 135.f : -135.f;
		else
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + bFlipYaw ? -135.f : 135.f;
	}
	void FakeTwoStepCustom(CUserCmd* pCmd, bool& bSendPacket)
	{


		float AddLby = Menu::Window.RageBotTab.AddLBYYAWFLIP.GetValue();
	
		


		static bool bFlipYaw;
		float flInterval = Interfaces::Globals->interval_per_tick;
		float flTickcount = pCmd->tick_count;
		float flTime = flInterval * flTickcount;
		if (std::fmod(flTime, 1) == 0.f)
			bFlipYaw = !bFlipYaw;
		if (Menu::Window.RageBotTab.MinusLBYADD.GetState())
		{
			if (bSendPacket)
				pCmd->viewangles.y += bFlipYaw ? AddLby * -1 : AddLby;
			else
				pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + bFlipYaw ? AddLby : AddLby * -1;
		}
		else {
			if (bSendPacket)
				pCmd->viewangles.y += bFlipYaw ? AddLby : AddLby * -1;
			else
				pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + bFlipYaw ? AddLby * -1 : AddLby;
		}
	}
	void FastSpint(CUserCmd *pCmd)
	{
		int random = rand() % 100;
		int random2 = rand() % 1000;

		static bool dir;
		static float current_y = pCmd->viewangles.y;

		if (random == 1) dir = !dir;

		if (dir)
			current_y += 100;
		else
			current_y -= 100;

		pCmd->viewangles.y = current_y;

		if (random == random2)
			pCmd->viewangles.y += random;
	}

	void SlowSpin(CUserCmd *pCmd)
	{
		int random = rand() % 100;
		int random2 = rand() % 1000;

		static bool dir;
		static float current_y = pCmd->viewangles.y;

		if (random == 1) dir = !dir;

		if (dir)
			current_y += 10;
		else
			current_y -= 10;

		pCmd->viewangles.y = current_y;

		if (random == random2)
			pCmd->viewangles.y += random;

	}
	void fakebackwards(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y -= 180; // this is real yaw
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y -= 90; // this is fake aa
			ChokedPackets = -1;
		}
	}

	void BackJitter(CUserCmd *pCmd)
	{
		int random = 179 - rand() % 100;
		static float current_y = pCmd->viewangles.y;
		current_y += random;
		pCmd->viewangles.y = current_y;

	}
	void MoveFix(CUserCmd *cmd, Vector &realvec)
	{
		Vector vMove(cmd->forwardmove, cmd->sidemove, cmd->upmove);
		float flSpeed = sqrt(vMove.x * vMove.x + vMove.y * vMove.y), flYaw;
		Vector vMove2;
		VectorAngles(vMove, vMove2);

		flYaw = DEG2RAD(cmd->viewangles.y - realvec.y + vMove2.y);
		cmd->forwardmove = cos(flYaw) * flSpeed;
		cmd->sidemove = sin(flYaw) * flSpeed;

		if (cmd->viewangles.x < -90.f || cmd->viewangles.x > 90.f)
			cmd->forwardmove = -cmd->forwardmove;
	}

	void Flip(CUserCmd *pCmd)
	{
		static bool back = false;
		back = !back;
		if (back)
			pCmd->viewangles.y -= rand() % 100;
		else
			pCmd->viewangles.y += rand() % 100;

	}
}

void CRageBot::aimAtPlayer(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

	if (!pLocal || !pWeapon)
		return;

	Vector eye_position = pLocal->GetEyePosition();

	float best_dist = pWeapon->GetCSWpnData()->m_flRange;

	IClientEntity* target = nullptr;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			if (Globals::TargetID != -1)
				target = Interfaces::EntList->GetClientEntity(Globals::TargetID);
			else
				target = pEntity;

			Vector target_position = target->GetEyePosition();

			float temp_dist = eye_position.DistTo(target_position);

			if (best_dist > temp_dist)
			{
				best_dist = temp_dist;
				CalcAngle(eye_position, target_position, pCmd->viewangles);
			}
		}
	}
}

void VectorAngles2(const Vector &vecForward, Vector &vecAngles)
{
	Vector vecView;
	if (vecForward[1] == 0.f && vecForward[0] == 0.f)
	{
		vecView[0] = 0.f;
		vecView[1] = 0.f;
	}
	else
	{
		vecView[1] = atan2(vecForward[1], vecForward[0]) * 180.f / M_PI;

		if (vecView[1] < 0.f)
			vecView[1] += 360.f;

		vecView[2] = sqrt(vecForward[0] * vecForward[0] + vecForward[1] * vecForward[1]);

		vecView[0] = atan2(vecForward[2], vecView[2]) * 180.f / M_PI;
	}

	vecAngles[0] = -vecView[0];
	vecAngles[1] = vecView[1];
	vecAngles[2] = 0.f;
}
void AngleVectors2(const Vector& qAngles, Vector& vecForward)
{
	float sp, sy, cp, cy;
	SinCos((float)(qAngles[1] * (M_PI / 180.f)), &sy, &cy);
	SinCos((float)(qAngles[0] * (M_PI / 180.f)), &sp, &cp);

	vecForward[0] = cp*cy;
	vecForward[1] = cp*sy;
	vecForward[2] = -sp;
}

namespace AntiAims // CanOpenFire checks for fake anti aims?
{


	/*
	________________________________________________________
	______             ______
	/      |           /      \
	$$$$$$/  _______  /$$$$$$  |______    _______
	$$ |  /       \ $$ |_ $$//      \  /       |
	$$ |  $$$$$$$  |$$   |  /$$$$$$  |/$$$$$$$/
	$$ |  $$ |  $$ |$$$$/   $$ |  $$ |$$      \
	_$$ |_ $$ |  $$ |$$ |    $$ \__$$ | $$$$$$  |
	/ $$   |$$ |  $$ |$$ |    $$    $$/ /     $$/
	$$$$$$/ $$/   $$/ $$/      $$$$$$/  $$$$$$$/
	________________________________________________________
	*/
	/*
	- 4 Boxen namen siehe unten
	- bSendPacket = true  <-- Fake Angle
	- bSendPacket = false <-- True Angle
	*/

	void Emotion(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = 89.000000f;
	}

	void Down(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = 179.000000f;
	}

	void Up(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = -179.000000f;
	}

	void FakeDown1(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool flip = false;
		if (flip)
		{
			pCmd->viewangles.x = -179.000000f;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.x = 179.000000f;
			bSendPacket = true;
		}
	}

	void FakeUp1(CUserCmd *pCmd, bool &bSendPacket)
	{
		pCmd->viewangles.x = 89.000000f;
	}

	void Zero(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = 0.f;
	}

	void FakeZero(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.x = -89;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.x = 0.f;
			ChokedPackets = -1;
		}
	}

	void Mixed(CUserCmd *pCmd)
	{
		static float pDance = 0.0f;

		pDance += 45.0f;
		if (pDance > 100)
			pDance = 0.0f;
		else if (pDance > 75.f)
			pCmd->viewangles.x = -11.295f;
		else if (pDance < 45.f)
			pCmd->viewangles.x = 26.3491651f;
		else if (pDance < 65)
			pCmd->viewangles.x = -9.91545f;
		else if (pDance < 75.f)
			pCmd->viewangles.x = 67.945324f;
		else if (pDance < 89.f)
			pCmd->viewangles.x = -72.62294519f;
		else if (pDance < 85.f)
			pCmd->viewangles.x = 35.19245635f;
	}

	void LegitFake(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool choke = false;
		if (choke)
		{
			bSendPacket = false; //true angle
			pCmd->viewangles.x = -90.f;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.x = 0.f; //fake angle
		}
		choke = !choke;
	}

	void LegitFakeJitter(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool choke = false;
		static bool s = false;
		if (choke)
		{
			bSendPacket = false; //true angle
			if (s) {
				pCmd->viewangles.x = 90.f;
			}
			else {
				pCmd->viewangles.x = -90.f;
			}
			s = !s;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.x = 0.f; //fake angle
		}
		choke = !choke;
	}

	// YAW

	#define RandomInt(min, max) (rand() % (max - min + 1) + min)

	void Backwards(CUserCmd *pCmd)
	{
		pCmd->viewangles.y -= 180.f;
	}

	void Static(CUserCmd *pCmd)
	{
		static bool flip = false;
		static bool flip2 = false;
		flip + !flip;
		flip2 = !flip2;
		{
			if (flip)
			{

				if (flip2)
					pCmd->viewangles.y += 90.000000f;

				else
					pCmd->viewangles.y -= 90.000000f;
			}
			else
			{
				pCmd->viewangles.y += 180.000000f;
			}
		}

	}

	void Static2(CUserCmd *pCmd)
	{
		static bool flip = false;
		static bool flip2 = false;
		flip + !flip;
		flip2 = !flip2;
		{
			if (flip)
			{
				if (flip2)
				{
					pCmd->viewangles.y += 90.000000f;
				}
				else
					pCmd->viewangles.y -= 90.000000f;
			}
			else
			{
				if (flip2)
					pCmd->viewangles.y -= 162.000000f;
				else if (!flip2)
					pCmd->viewangles.y -= 180.000000f;
			}
		}
	}

	void Static3(CUserCmd *pCmd)
	{
		static bool flip = false;
		static bool flip2 = false;
		flip + !flip;
		flip2 = !flip2;
		{
			if (flip)
			{
				if (flip2)
				{
					pCmd->viewangles.y += 90.000000f;
				}
				else
					pCmd->viewangles.y -= 90.000000f;
			}
			else
			{
				if (flip2)
					pCmd->viewangles.y += 90.000000f;
				else if (!flip2)
					pCmd->viewangles.y -= 180.000000f;
			}
		}
	}


	void Twitch1(CUserCmd *pCmd)
	{
			int random = rand() % 100;

			if (random < 98)
				pCmd->viewangles.y -= 180;

			if (random < 15)
			{
				float change = -70 + (rand() % (int)(140 + 1));
				pCmd->viewangles.y += change;
			}
			if (random == 69)
			{
				float change = -90 + (rand() % (int)(180 + 1));
				pCmd->viewangles.y += change;
			}
	}
	

	void ZeroYaw(CUserCmd *pCmd)
	{
		pCmd->viewangles.y = 0.f;
	}

	void Spinbot(CUserCmd *pCmd)
	{
		int meme = Menu::Window.RageBotTab.AntiAimSpinspeed.GetValue() * 1000.0;
		float CalculatedCurTime_1 = (Interfaces::Globals->curtime * meme);
		pCmd->viewangles.y = CalculatedCurTime_1;
	}

	void LBYBreaker(CUserCmd *pCmd)
	{
		static bool wilupdate;
		static float LastLBYUpdateTime = 0;
		IClientEntity* pLocal = hackManager.pLocal();
		float server_time = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
		if (server_time >= LastLBYUpdateTime)
		{
			LastLBYUpdateTime = server_time + 1.125f;
			wilupdate = true;
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + RandomInt(30, 61);
		}
		else
		{
			wilupdate = false;
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - RandomInt(180, 360);
		}

	}

	
	void FlipAA(CUserCmd *pCmd)
	{
		static bool bFlip = false;
		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 1.09f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		static int state = 0;
		float yaw;
		if (flCurTime >= flNextTimeUpdate) {
			bFlip = !bFlip;
			state = 0;
		}
		if (bFlip) {
			yaw = 110.f + state * 34.f;
			if (state < 3)
				state++;
		}
		else {
			yaw = -110.f - state * 34.f;
			if (state < 3)
				state++;
		}
		pCmd->viewangles.y = yaw;
	}

	void FlipAAKidua(CUserCmd *pCmd)
	{
		static bool bFlip = false;
		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 1.09f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		static int state = 0;
		float yaw;
		if (flCurTime >= flNextTimeUpdate) {
			bFlip = !bFlip;
			state = 0;
		}

		if (bFlip) {
			yaw = 90.f + state * 34.f;
			if (state > 1)
				yaw = 90.f + 34.8f;
		}
		else {
			yaw = -90.f - state * 34.f;
			if (state > 1)
				yaw = -90.f - 34.8f;
		}
		pCmd->viewangles.y = yaw;
	}
	void LispAA(CUserCmd *pCmd)
	{
		static bool bFlip = false;
		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 1.09f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		static int state = 0;
		float yaw;
		if (flCurTime >= flNextTimeUpdate) {
			bFlip = !bFlip;
			state = 0;
		}
		static bool ySwitch = false;

		if (ySwitch) {
			yaw = 322210000;
		}
		else {
			yaw = -322210000;
		}

		ySwitch = !ySwitch;
		pCmd->viewangles.y = yaw;
	}
	void Lowerbody(CUserCmd *pCmd)
	{
		static bool wilupdate;
		static float LastLBYUpdateTime = 0;
		IClientEntity* pLocal = hackManager.pLocal();
		float server_time = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
		if (server_time >= LastLBYUpdateTime)
		{
			LastLBYUpdateTime = server_time + 1.125f;
			wilupdate = true;
			pCmd->viewangles.y -= 90.f;
		}
		else
		{
			wilupdate = false;
			pCmd->viewangles.y += 90.f;
		}
	}

	// Fake Yaw

	void FakeBackwards(CUserCmd *pCmd, bool &bSendPacket)
	{
		bSendPacket = true;
		pCmd->viewangles.y -= 180.f;
	}

	void FakeSpinbot(CUserCmd *pCmd, bool &bSendPacket)
	{
		bSendPacket = true;
		int meme = Menu::Window.RageBotTab.AntiAimSpinspeed.GetValue() * 1000.0;
		float CalculatedCurTime_1 = (Interfaces::Globals->curtime * meme);
		pCmd->viewangles.y = CalculatedCurTime_1;
	}
	
	void FakeSpin111(CUserCmd *pCmd, bool &bSendPacket)
	{
		int Add = 0;
		IClientEntity* pLocal = hackManager.pLocal();
		if (bSendPacket)
		{
			if (pLocal->GetVelocity().Length2D() > 1)
			{
				if (!(pLocal->GetFlags() & FL_ONGROUND))
				{
					Add = 90;
				}
				else
				{
					Add = -90;
				}
			}
			else
			{
				Add = 170;
			}
		}
		else
		{
			if (!(pLocal->GetFlags() & FL_ONGROUND))
			{
				Add = -45;
			}
			else
			{
				if (pLocal->GetVelocity().Length2D() > 1)
				{
					Add = -161;
				}
				else
				{
					Add = -20;
				}
			}
		}
		Vector View = pCmd->viewangles;
		pCmd->viewangles.y = View.y + Add;
	}
	void FakeLowerbody(CUserCmd *pCmd, bool &bSendPacket)
	{
		bSendPacket = true;
		static bool wilupdate;
		static float LastLBYUpdateTime = 0;
		IClientEntity* pLocal = hackManager.pLocal();
		float server_time = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
		if (server_time >= LastLBYUpdateTime)
		{
			LastLBYUpdateTime = server_time + 1.125f;
			wilupdate = true;
			pCmd->viewangles.y -= 90.f;
		}
		else
		{
			wilupdate = false;
			pCmd->viewangles.y += 90.f;
		}
	}
	int MyLowerbody;
	void Fakethree2(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool flip;
		static bool flip2;

		static bool yFlip;
		yFlip = !yFlip;
		bSendPacket = yFlip;

		float flip2angle = 0.f;
		if (hackManager.pLocal()->GetVelocity().Length2D() > 1 && hackManager.pLocal()->GetFlags() & FL_ONGROUND) {
			pCmd->viewangles.y += 179.0f;
		}
		else
		{
			if (hackManager.pLocal()->pelvisangs() != MyLowerbody)
				flip2 = !flip2;
			if (flip2)
				flip2angle = 180.f;
			else
				flip2angle = 0.f;


			if (flip)
			{
				pCmd->viewangles.y += 90.0f + flip2angle;
				bSendPacket = false;
			}
			else
			{
				pCmd->viewangles.y += -90.0f + flip2angle;
				bSendPacket = true;
			}
			MyLowerbody = hackManager.pLocal()->pelvisangs();
			flip = !flip;
		}
	}
	void Fakethree(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool flip;
		static bool flip2;

		static bool yFlip;
		yFlip = !yFlip;
		bSendPacket = yFlip;

		float flip2angle = 0.f;
		if (hackManager.pLocal()->GetVelocity().Length2D() > 1 && hackManager.pLocal()->GetFlags() & FL_ONGROUND) {
			pCmd->viewangles.y += 179.0f;
		}
		else
		{
			if (hackManager.pLocal()->pelvisangs() != MyLowerbody)
				flip2 = !flip2;
			if (flip2)
				flip2angle = 180.f;
			else
				flip2angle = 0.f;


			if (flip)
			{
				pCmd->viewangles.y += 90.0f + flip2angle;
				bSendPacket = false;
			}
			else
			{
				pCmd->viewangles.y += -90.0f + flip2angle;
				bSendPacket = true;
			}
			MyLowerbody = hackManager.pLocal()->pelvisangs();
			flip = !flip;
		}
	}
	void LBYBreakerFake(CUserCmd *pCmd, bool &bSendPacket)
	{
		bSendPacket = true;
		static bool wilupdate;
		static float LastLBYUpdateTime = 0;
		IClientEntity* pLocal = hackManager.pLocal();
		float server_time = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
		if (server_time >= LastLBYUpdateTime)
		{
			LastLBYUpdateTime = server_time + 1.125f;
			wilupdate = true;
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + RandomInt(30, 61);
		}
		else
		{
			wilupdate = false;
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - RandomInt(180, 360);
		}

	}
};

bool EdgeAntiAim(IClientEntity* pLocalBaseEntity, CUserCmd* cmd, float flWall, float flCornor)
{
	Ray_t ray;
	trace_t tr;

	CTraceFilter traceFilter;
	traceFilter.pSkip = pLocalBaseEntity;

	auto bRetVal = false;
	auto vecCurPos = pLocalBaseEntity->GetEyePosition();

	for (float i = 0; i < 360; i++)
	{
		Vector vecDummy(10.f, cmd->viewangles.y, 0.f);
		vecDummy.y += i;

		NormalizeVector(vecDummy);

		Vector vecForward;
		AngleVectors2(vecDummy, vecForward);

		auto flLength = ((16.f + 3.f) + ((16.f + 3.f) * sin(DEG2RAD(10.f)))) + 7.f;
		vecForward *= flLength;

		ray.Init(vecCurPos, (vecCurPos + vecForward));
		Interfaces::Trace->TraceRay(ray, MASK_SHOT, (CTraceFilter *)&traceFilter, &tr);

		if (tr.fraction != 1.0f)
		{
			Vector qAngles;
			auto vecNegate = tr.plane.normal;

			vecNegate *= -1.f;
			VectorAngles2(vecNegate, qAngles);

			vecDummy.y = qAngles.y;

			NormalizeVector(vecDummy);
			trace_t leftTrace, rightTrace;

			Vector vecLeft;
			AngleVectors2(vecDummy + Vector(0.f, 30.f, 0.f), vecLeft);

			Vector vecRight;
			AngleVectors2(vecDummy - Vector(0.f, 30.f, 0.f), vecRight);

			vecLeft *= (flLength + (flLength * sin(DEG2RAD(30.f))));
			vecRight *= (flLength + (flLength * sin(DEG2RAD(30.f))));

			ray.Init(vecCurPos, (vecCurPos + vecLeft));
			Interfaces::Trace->TraceRay(ray, MASK_SHOT, (CTraceFilter*)&traceFilter, &leftTrace);

			ray.Init(vecCurPos, (vecCurPos + vecRight));
			Interfaces::Trace->TraceRay(ray, MASK_SHOT, (CTraceFilter*)&traceFilter, &rightTrace);

			if ((leftTrace.fraction == 1.f) && (rightTrace.fraction != 1.f))
				vecDummy.y -= flCornor; // left
			else if ((leftTrace.fraction != 1.f) && (rightTrace.fraction == 1.f))
				vecDummy.y += flCornor; // right			

			cmd->viewangles.y = vecDummy.y;
			cmd->viewangles.y -= flWall;
			cmd->viewangles.x = 89.f;
			bRetVal = true;
		}
	}
	return bRetVal;
}

// AntiAim
void CRageBot::DoAntiAim(CUserCmd *pCmd, bool &bSendPacket) // pCmd->viewangles.y = 0xFFFFF INT_MAX or idk
{
	IClientEntity* pLocal = hackManager.pLocal();
	IClientEntity *pEntity;
	IClientEntity* pLocalEntity;

	if ((pCmd->buttons & IN_USE) || pLocal->GetMoveType() == MOVETYPE_LADDER)
		return;
	
	// If the aimbot is doing something don't do anything
	if ((IsAimStepping || pCmd->buttons & IN_ATTACK) && !Menu::Window.RageBotTab.AimbotSlientSelection.GetIndex())
		return;

	// Weapon shit
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	if (pWeapon)
	{
		CSWeaponInfo* pWeaponInfo = pWeapon->GetCSWpnData();
		// Knives or grenades
		if (!GameUtils::IsBallisticWeapon(pWeapon))
		{
			if (Menu::Window.RageBotTab.AntiAimKnife.GetState())
			{
				if (!CanOpenFire() || pCmd->buttons & IN_ATTACK2)
					return;
			}
			else
			{
				return;
			}
		}
	}

	if (Menu::Window.RageBotTab.AntiAimEdge.GetState()) {
		auto bEdge = EdgeAntiAim(hackManager.pLocal(), pCmd, 360.f, 45.f);
		if (bEdge)
			return;
	}

	if (Menu::Window.RageBotTab.AntiAimTarget.GetState())
	{
		aimAtPlayer(pCmd);
	}

	// Anti-Aim Pitch
	switch (Menu::Window.RageBotTab.AntiAimPitch.GetIndex())
	{
	case 0:
		// No AA
		break;
	case 1:
		// Emotion
		AntiAims::Emotion(pCmd);
		break;
	case 2:
		// Down
		AntiAims::Down(pCmd);
		break;
	case 3:
		// Up
		AntiAims::Up(pCmd);
		break;
	case 4:
		// Fake Down
		AntiAims::FakeDown1(pCmd, bSendPacket);
		break;
	case 5:
		// Fake Up
		AntiAims::FakeUp1(pCmd, bSendPacket);
		break;
	case 6:
		// Zero
		AntiAims::Zero(pCmd);
		break;
	case 7:
		// Fake Zero
		AntiAims::FakeZero(pCmd, bSendPacket);
		break;
	case 8:
		// Mixed
		AntiAims::Mixed(pCmd);
		break;
	}
	bool RealAndFake;
	switch (Menu::Window.RageBotTab.AntiAimReady.GetIndex())
	{
	case 0:
		RealAndFake = true;
		break;
	case 1:
		// Backward
		AntiAims::FakeLowerBody135(pCmd, bSendPacket);
		RealAndFake = false;
		break;
	case 2:
		// Static
		AntiAims::LBYJitter(pCmd, bSendPacket);
		RealAndFake = false;
		break;
	case 3:
		// Static 2
		AntiAims::Backward(pCmd);
		RealAndFake = false;
		break;
	case 4:
		// Static 3
		AntiAims::FakeBackward(pCmd, bSendPacket);
		RealAndFake = false;
		break;
	case 5:
		// Spinbot
		AntiAims::FakeTwoStep(pCmd, bSendPacket);
		RealAndFake = false;
		break;
	case 6:
		// Spinbot
		AntiAims::FakeSideways(pCmd, bSendPacket);
		RealAndFake = false;
		break;
	case 7:
		// Lower Body
		AntiAims::FakeTwoStepCustom(pCmd, bSendPacket);
		RealAndFake = false;
		break;
	case 8:
		// LBY Breaker
		AntiAims::FakeSpin111(pCmd, bSendPacket);
		RealAndFake = false;
		break;
/*	case 8:
		// LBY Breaker
		AntiAims::LBYBreaker(pCmd);
		break;*/
	}
	//Anti-Aim Yaw
	if (RealAndFake) {
		switch (Menu::Window.RageBotTab.AntiAimYaw.GetIndex())
		{
		case 0:
			// No Yaw AA
			break;
		case 1:
			// Backward
			AntiAims::Backwards(pCmd);
			break;
		case 2:
			// Static
			AntiAims::Static(pCmd);
			break;
		case 3:
			// Static 2
			AntiAims::Static2(pCmd);
			break;
		case 4:
			// Static 3
			AntiAims::Static3(pCmd);
			break;
		case 5:
			// Spinbot
			AntiAims::Spinbot(pCmd);
			break;
		case 6:
			// Spinbot
			AntiAims::Twitch1(pCmd);
			break;
		case 7:
			// Lower Body
			AntiAims::Lowerbody(pCmd);
			break;
		case 8:
			// LBY Breaker
			AntiAims::LBYBreaker(pCmd);
			break;
		case 9:
			// LBY Breaker
			AntiAims::FlipAA(pCmd);
			break;
		case 10:
			// LBY Breaker
			AntiAims::FlipAAKidua(pCmd);
			break;
		case 11:
			// LBY Breaker
			AntiAims::LispAA(pCmd);
			break;
		case 12:
			// LBY Breaker
			AntiAims::FakeSpin111(pCmd, bSendPacket);
			break;
		case 13:
			// Fake 3 for Yaw
			AntiAims::Fakethree(pCmd, bSendPacket);
			break;




		}

		switch (Menu::Window.RageBotTab.AntiAimYawFake.GetIndex())
		{
		case 0:
			// No Yaw AA
			break;
		case 1:
			// Backward
			AntiAims::FakeBackwards(pCmd, bSendPacket);
			break;
		case 2:
			// Spinbot
			AntiAims::FakeSpinbot(pCmd, bSendPacket);
			break;
		case 3:
			// Lower Body
			AntiAims::FakeLowerbody(pCmd, bSendPacket);
			break;
		case 4:
			// LBY Breaker
			AntiAims::LBYBreakerFake(pCmd, bSendPacket);
			break;
		case 5:
			// LBY Breaker
			AntiAims::FlipAA(pCmd);
			break;
		case 6:
			// LBY Breaker
			AntiAims::FlipAAKidua(pCmd);
			break;
		case 7:
			// LBY Breaker
			AntiAims::LispAA(pCmd);
			break;
		case 8:
			// LBY Breaker
			AntiAims::FakeSpin111(pCmd, bSendPacket);
			break;
		case 9:
			// LBY Breaker
			AntiAims::Fakethree2(pCmd, bSendPacket);
			break;

		}
	}
	if (Menu::Window.RageBotTab.AntiAimJitterLBY.GetState())
	{
		static bool antiResolverFlip = false;
		if (pCmd->viewangles.y == pLocal->GetLowerBodyYaw())
		{
			if (antiResolverFlip)
				pCmd->viewangles.y += 60.f;
			else
				pCmd->viewangles.y -= 60.f;

			antiResolverFlip = !antiResolverFlip;
		}
	}
}