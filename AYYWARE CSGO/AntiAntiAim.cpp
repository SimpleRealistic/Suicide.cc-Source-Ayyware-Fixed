#include "Interfaces.h"
#include "Menu.h"
#include "AntiAntiAim.h"
#include "Resolvermeme.h"
#include "MiscDefinitions.h"
#include "Entities.h"

int Globals::Shots = 0;
bool Globals::change;
CUserCmd* Globals::UserCmd;
int Globals::TargetID;
IClientEntity* Globals::Target;
float YawDelta[64];
float reset[64];
float Delta[64];
float OldLowerBodyYaw[64];
float Resolved_angles[64];
int iSmart;
static int jitter = -1;
void Normalize(float& angle) {
	while (angle > 180.f) {
		angle -= 360.f;
	}
	while (angle < -180.f) {
		angle += 360.f;
	}
}



void R::Resolver()
{
	
	// This is basically a bruteforce resolver
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	
	for (int i = 0; i < Interfaces::Engine->GetMaxClients(); ++i)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);

		if (!pEntity || pEntity->IsDormant() || !pEntity->IsAlive())
			continue;

		if (pEntity->GetTeamNum() == pLocal->GetTeamNum() || !pLocal->IsAlive())
			continue;

		Vector* eyeAngles = pEntity->GetEyeAnglesPointer();

		if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex()  || Menu::Window.RageBotTab.AccuracyResolverPitch.GetIndex() || Menu::Window.RageBotTab.AccuracyResolverAuto.GetIndex())
		{
			if (pEntity != Globals::Target) continue;

			Vector AimAngle;
			CalcAngleYawOnly(pEntity->GetEyePosition(), pLocal->GetAbsOrigin2(), AimAngle);
			NormalizeVector(AimAngle);
			static float StoredYaw = 0;
			static bool bLowerBodyIsUpdated = false;
			if (pEntity->GetLowerBodyYaw() != StoredYaw) bLowerBodyIsUpdated = true;
			else bLowerBodyIsUpdated = false;
			if (bLowerBodyIsUpdated) StoredYaw = pEntity->GetLowerBodyYaw();
			if (pEntity->GetFlags() & FL_ONGROUND && pEntity->GetVelocity().Length2D() != 0) eyeAngles->y = pEntity->GetLowerBodyYaw();
			else
			{
				
				if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 1) // LBY Bruteforce 2
				{
					int num = Globals::Shots % 4;
					switch (num) {
					case 1:eyeAngles->y = pEntity->GetLowerBodyYaw() - 15; break;
					case 2:eyeAngles->y = pEntity->GetLowerBodyYaw() + 40; break;
					case 3:eyeAngles->y = pEntity->GetLowerBodyYaw() - 40; break;
					case 4:eyeAngles->y = pEntity->GetLowerBodyYaw() + 15; break;
					}
				}
				
				if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 2) // Normal Bruteforce
				{
					int num = Globals::Shots % 5;
					switch (num) {
					case 0:eyeAngles->y = AimAngle.y - 0; break;
					case 1:eyeAngles->y = AimAngle.y - 15; break;
					case 2:eyeAngles->y = AimAngle.y + 40; break;
					case 3:eyeAngles->y = AimAngle.y - 40; break;
					case 4:eyeAngles->y = AimAngle.y + 15; break;
					}
				}
				if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 3) // Auto 1
				{
					for (int i = 0; i < 64; ++i)
					{
						IClientEntity* ent = Interfaces::EntList->GetClientEntity(i);

						if (!ent || !ent->IsAlive())
							continue;

						*(float*)((uintptr_t)ent + 0xAA08 + 4) += Delta[i];
						reset[i] = *(float*)((uintptr_t)ent + 0xAA08 + 4);

						float LowerBodyYaw = *(float*)((uintptr_t)ent + 0x39F8);

						if (OldLowerBodyYaw[i] != LowerBodyYaw)
						{
							YawDelta[i] = reset[i] - LowerBodyYaw;
							OldLowerBodyYaw[i] = LowerBodyYaw;
							*(float*)((uintptr_t)ent + 0xAA08 + 4) = LowerBodyYaw;
						}
						else
						{
							*(float*)((uintptr_t)ent + 0xAA08 + 4) = reset[i] - YawDelta[i];

							while (*(float*)((uintptr_t)ent + 0xAA08 + 4) < -180.0f)
								*(float*)((uintptr_t)ent + 0xAA08 + 4) += 360.0f;

							while (*(float*)((uintptr_t)ent + 0xAA08 + 4) > 180.0f)
								*(float*)((uintptr_t)ent + 0xAA08 + 4) -= 360.0f;
						}
						Resolved_angles[i] = *(float*)((uintptr_t)ent + 0xAA08 + 4);
					}
				}
				if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 4)
				{
					static float lowerDelta[64];
					static float lastYaw[64];

					IClientEntity *ent = Interfaces::EntList->GetClientEntity(i);

					float curLower = ent->GetLowerBodyYaw();
					float curYaw = ent->GetEyeAngles().y;

					lowerDelta[i] = curYaw - curLower;

					if (fabs(lowerDelta[i]) > 15.f)
					{
						if (ent->isMoving())
						{
							eyeAngles->y = ent->GetLowerBodyYaw();
						}
						else {
							if (curYaw != lastYaw[i])
							{
								eyeAngles->y += lowerDelta[i];
								lastYaw[i] = curYaw;
							}
						}
					}
				}
				if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 5)
				{
					static float OldLowerBodyYaws[64];
					static float OldYawDeltas[64];

					IClientEntity *ent = Interfaces::EntList->GetClientEntity(i);

					float CurYaw = ent->GetLowerBodyYaw();
					if (OldLowerBodyYaws[i] != CurYaw) {
						OldYawDeltas[i] = ent->GetEyeAngles().y - CurYaw;
						OldLowerBodyYaws[i] = CurYaw;
						eyeAngles->y = CurYaw;
						continue;
					}
					else {
						eyeAngles->y = ent->GetEyeAngles().y - OldYawDeltas[i];
					}
				}
				if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 6)
				{
					static bool isMoving;
					float PlayerIsMoving = abs(pEntity->GetVelocity().Length());
					if (PlayerIsMoving > 0.1) isMoving = true;
					else if (PlayerIsMoving <= 0.1) isMoving = false;

					static float StoredLBY = 0.0f;
					static bool bLowerBodyIsUpdated;
					if (pEntity->GetLowerBodyYaw() != StoredLBY) bLowerBodyIsUpdated = true;
					else bLowerBodyIsUpdated = false;

					float bodyeyedelta = pEntity->GetEyeAngles().y - pEntity->GetLowerBodyYaw();

					if (bLowerBodyIsUpdated || isMoving || fabsf(bodyeyedelta) >= 35.0f)
					{
						eyeAngles->y = pEntity->GetLowerBodyYaw();
						StoredLBY = pEntity->GetLowerBodyYaw();
					}

					else
					{
						if (fabsf(bodyeyedelta) < 35.0f && fabsf(bodyeyedelta) > 0.0f)
							eyeAngles->y = pEntity->GetLowerBodyYaw() + bodyeyedelta;
					}
				}
				

				if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 7) {
					static float lowerDelta[64];
					static float lastYaw[64];

					float curLower = pEntity->GetLowerBodyYaw();
					float curYaw = pEntity->GetEyeAngles().y;
					float curYaw1 = pEntity->GetEyeAngles().y;
					lowerDelta[i] = curYaw - curLower;
					Normalize(lowerDelta[i]);

					if (fabs(lowerDelta[i]) > 15.f) {
						if (pEntity->isMoving()) {
							curYaw1 = pEntity->GetLowerBodyYaw();
						}
						else {
							if (curYaw != lastYaw[i]) {
								curYaw1 += lowerDelta[i];
								lastYaw[i] = curYaw;
							}
						}
					}
				}
			}
			if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 2) // Normal Bruteforce
			{
				int num = Globals::Shots % 5;
				switch (num) {
				case 0:eyeAngles->y = AimAngle.y - 0; break;
				case 1:eyeAngles->y = AimAngle.y - 15; break;
				case 2:eyeAngles->y = AimAngle.y + 40; break;
				case 3:eyeAngles->y = AimAngle.y - 40; break;
				case 4:eyeAngles->y = AimAngle.y + 15; break;
				}
			}
			// Pitch Resolver
			if (Menu::Window.RageBotTab.AccuracyResolverPitch.GetIndex() == 1) // Test 1
			{
				int Shot1 = Menu::Window.RageBotTab.Shot1.GetValue();
				int Shot2 = Menu::Window.RageBotTab.Shot2.GetValue();
				int Shot3 = Menu::Window.RageBotTab.Shot3.GetValue();
				int Shot4 = Menu::Window.RageBotTab.Shot4.GetValue();
				int Shot5 = Menu::Window.RageBotTab.Shot5.GetValue();

				int num = Globals::Shots % 5;
				switch (num) {
				case 0:eyeAngles->x = AimAngle.x - Shot1; break;
				case 1:eyeAngles->x = AimAngle.x - Shot2; break;
				case 2:eyeAngles->x = AimAngle.x + Shot3; break;
				case 3:eyeAngles->x = AimAngle.x - Shot4; break;
				case 4:eyeAngles->x = AimAngle.x + Shot5; break;
				}

			}
		}
	}
}

void FixY(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	static Vector vLast[65];
	static bool bShotLastTime[65];
	static bool bJitterFix[65];

	float *flPitch = (float*)((DWORD)pOut - 4);
	float flYaw = pData->m_Value.m_Float;
	bool bHasAA;
	bool bSpinbot;

	if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() > 0)
	{
		int value = rand() % 3 + 0;

		switch (value)
		{
		case 0:
		{
			flYaw = (rand() % 180);
			break;
		}
		case 1:
		{
			flYaw = (rand() % 360);
			break;
		}
		case 2:
		{
			flYaw = 0;
			break;
		}
		}
	}
}

// Simple Resolver for Fake Down
void FixX(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	float* ang = (float*)pOut;
	*ang = pData->m_Value.m_Float;

	if (!Menu::Window.RageBotTab.AccuracyResolverPitch.GetIndex() == 1) return;

	if (pData->m_Value.m_Float > 180.0f)
		*ang -= 360.0f;
	else if (pData->m_Value.m_Float < -180.0f)
		*ang += 360.0f;

	if (pData->m_Value.m_Float > 89.0f && pData->m_Value.m_Float < 91.0f)
		*ang -= 90.0f;
	else if (pData->m_Value.m_Float > -89.0f && pData->m_Value.m_Float < -91)
		*ang += 90.0f;
}

RecvVarProxyFn oRecvnModelIndex;

void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	// Get the knife view model id's
	int default_t = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	int default_ct = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
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

	// Get local player (just to stop replacing spectators knifes)
	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (Menu::Window.SkinTab.SkinEnable.GetState() && pLocal)
	{
		// If we are alive and holding a default knife(if we already have a knife don't worry about changing)
		if (pLocal->IsAlive() && (
			pData->m_Value.m_Int == default_t ||
			pData->m_Value.m_Int == default_ct ||
			pData->m_Value.m_Int == iBayonet ||
			pData->m_Value.m_Int == iButterfly ||
			pData->m_Value.m_Int == iFlip ||
			pData->m_Value.m_Int == iGunGame ||
			pData->m_Value.m_Int == iGut ||
			pData->m_Value.m_Int == iKarambit ||
			pData->m_Value.m_Int == iM9Bayonet ||
			pData->m_Value.m_Int == iHuntsman ||
			pData->m_Value.m_Int == iFalchion ||
			pData->m_Value.m_Int == iDagger ||
			pData->m_Value.m_Int == iBowie))
		{
			// Set whatever knife we want
			if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 0)
				pData->m_Value.m_Int = iBayonet;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 1)
				pData->m_Value.m_Int = iBowie;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 2)
				pData->m_Value.m_Int = iButterfly;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 3)
				pData->m_Value.m_Int = iFalchion;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 4)
				pData->m_Value.m_Int = iFlip;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 5)
				pData->m_Value.m_Int = iGut;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 6)
				pData->m_Value.m_Int = iHuntsman;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 7)
				pData->m_Value.m_Int = iKarambit;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 8)
				pData->m_Value.m_Int = iM9Bayonet;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 9)
				pData->m_Value.m_Int = iDagger;
		}
	}

	// Carry on the to original proxy
	oRecvnModelIndex(pData, pStruct, pOut);
}

void ApplyAAAHooks()
{
	ClientClass *pClass = Interfaces::Client->GetAllClasses();
	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
		if (!strcmp(pszName, "DT_CSPlayer"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;

				// Pitch Fix
				if (!strcmp(name, "m_angEyeAngles[0]"))
				{
					pProp->m_ProxyFn = FixX;
				}

				// Yaw Fix
				if (!strcmp(name, "m_angEyeAngles[1]"))
				{
					Utilities::Log("Yaw Fix Applied");
					pProp->m_ProxyFn = FixY;
				}
			}
		}
		else if (!strcmp(pszName, "DT_BaseViewModel"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;

				// Knives
				if (!strcmp(name, "m_nModelIndex"))
				{
					oRecvnModelIndex = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = Hooked_RecvProxy_Viewmodel;
				}
			}
		}
		pClass = pClass->m_pNext;
	}
}