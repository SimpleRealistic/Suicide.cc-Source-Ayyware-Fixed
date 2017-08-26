#include "Resolver.h"

int Globals::Shots = 0;
bool Globals::change;
CUserCmd* Globals::UserCmd;
int Globals::TargetID;
IClientEntity* Globals::Target;

void R::Resolve()
{
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	for (int i = 0; i < Interfaces::Engine->GetMaxClients(); ++i)
	{
		

		if (!pEntity || pEntity->IsDormant() || !pEntity->IsAlive())
			continue;

		if (pEntity->GetTeamNum() == pLocal->GetTeamNum() || !pLocal->IsAlive())
			continue;

		Vector* eyeAngles = pEntity->GetEyeAnglesPointer();

		if (Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 1 || Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 2)
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
				if (Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 1 || Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 2)
				{
					// Yaw Resolver
					// int num = Globals::Shots % 4 is a meme
					int num = Globals::Shots % 4;
					switch (num)
					{
					case 0:eyeAngles->y = eyeAngles->y + 180; break;
					case 1:eyeAngles->y = eyeAngles->y - 90; break;
					case 2:eyeAngles->y = eyeAngles->y + 90; break;
					case 3:eyeAngles->y = eyeAngles->y + 179.95; break;
					case 4:eyeAngles->y = eyeAngles->y - 179.95; break;
					case 5:eyeAngles->y = eyeAngles->y + 40; break;
					case 6:eyeAngles->y = eyeAngles->y - 40; break;
					case 7:eyeAngles->y = eyeAngles->y + 30; break;
					case 8:eyeAngles->y = eyeAngles->y - 30; break;
					case 9:eyeAngles->y = eyeAngles->y + 15; break;
					case 10:eyeAngles->y = eyeAngles->y - 15; break;
					}

					if (Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 2)
					{
						// Pitch Resolver
						std::string strPitch = std::to_string(eyeAngles->x);

						if (eyeAngles->x < -179.f) eyeAngles->x += 360.f;
						else if (eyeAngles->x > 90.0 || eyeAngles->x < -90.0) eyeAngles->x = 89.f; // Down
						else if (eyeAngles->x > 89.0 && eyeAngles->x < 91.0) eyeAngles->x -= 90.f; // Down
						else if (eyeAngles->x > 179.0 && eyeAngles->x < 181.0) eyeAngles->x -= 180;
						else if (eyeAngles->x > -179.0 && eyeAngles->x < -181.0) eyeAngles->x = 180;
						else if (fabs(eyeAngles->x) == 0) eyeAngles->x = std::copysign(89.0f, eyeAngles->x);
					}
				}
			}
		}
	}
}