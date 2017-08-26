#include "Visuals.h"
#include "Interfaces.h"
#include "RenderManager.h"

// Any init here
void CVisuals::Init()
{
	// Idk
}

// Don't really need to do anything in here
void CVisuals::Move(CUserCmd *pCmd, bool &bSendPacket) {}

// Main ESP Drawing loop
void CVisuals::Draw()
{
	NoScopeBorder();

	// Crosshair
	DrawCrosshair();

	// Recoil Crosshair
	DrawRecoilCrosshair();


	//NightMode();

}

// Draw a basic crosshair
void CVisuals::DrawCrosshair()
{
	if (Menu::Window.VisualsTab.OtherCrosshair.GetIndex() == 1)
	{
		static float rainbow;
		rainbow += 0.005f;
		if (rainbow > 1.f) rainbow = 0.f;

		RECT View = Render::GetViewport();
		int MidX = View.right / 2;
		int MidY = View.bottom / 2;
		Render::Line(MidX - 4, MidY - 4, MidX + 4, MidY + 4, Color::FromHSB(rainbow, 1.f, 1.f));
		Render::Line(MidX + 4, MidY - 4, MidX - 4, MidY + 4, Color::FromHSB(rainbow, 1.f, 1.f));
	}
	else if (Menu::Window.VisualsTab.OtherCrosshair.GetIndex() == 2)
	{
		static float rainbow;
		rainbow += 0.005f;
		if (rainbow > 1.f) rainbow = 0.f;

		RECT View = Render::GetViewport();
		int drX = View.right / 2;
		int drY = View.bottom / 2;
		// Color color(255, 0, 0, 255);

		Render::Line(drX, drY, drX, drY - 30, Color::FromHSB(rainbow, 1.f, 1.f));
		Render::Line(drX, drY - 30, drX + 30, drY - 30, Color::FromHSB(rainbow, 1.f, 1.f));

		Render::Line(drX, drY, drX + 30, drY, Color::FromHSB(rainbow, 1.f, 1.f));
		Render::Line(drX + 30, drY, drX + 30, drY + 30, Color::FromHSB(rainbow, 1.f, 1.f));

		Render::Line(drX, drY, drX, drY + 30, Color::FromHSB(rainbow, 1.f, 1.f));
		Render::Line(drX, drY + 30, drX - 30, drY + 30, Color::FromHSB(rainbow, 1.f, 1.f));

		Render::Line(drX, drY, drX - 30, drY, Color::FromHSB(rainbow, 1.f, 1.f));
		Render::Line(drX - 30, drY, drX - 30, drY - 30, Color::FromHSB(rainbow, 1.f, 1.f));
	}
	else if (Menu::Window.VisualsTab.OtherCrosshair.GetIndex() == 3)
	{
		// For future codes
	}
	else if (Menu::Window.VisualsTab.OtherCrosshair.GetIndex() == 4)
	{
		// For future codes
	}
	else if (Menu::Window.VisualsTab.OtherCrosshair.GetIndex() == 5)
	{
		// For future codes
	}
}

void CVisuals::BulletTrace(IClientEntity* pEntity, Color color)
{
	if (Menu::Window.VisualsTab.BulletTrace.GetState())
	{
		Vector src3D, dst3D, forward, src, dst;
		trace_t tr;
		Ray_t ray;
		CTraceFilter filter;

		AngleVectors(pEntity->GetEyeAngles(), &forward);
		filter.pSkip = pEntity;
		src3D = pEntity->GetBonePos(6) - Vector(0, 0, 0);
		dst3D = src3D + (forward * Menu::Window.VisualsTab.TraceLength.GetValue());

		ray.Init(src3D, dst3D);

		Interfaces::Trace->TraceRay(ray, MASK_SHOT, &filter, &tr);

		if (!Render::WorldToScreen(src3D, src) || !Render::WorldToScreen(tr.endpos, dst))
			return;

		Render::Line(src.x, src.y, dst.x, dst.y, color);
		Interfaces::Surface->DrawFilledRect(dst.x - 3, dst.y - 3, 6, 6);
	}
};

// Recoil crosshair
void CVisuals::DrawRecoilCrosshair()
{
	if (Menu::Window.VisualsTab.OtherRecoilCrosshair.GetIndex() == 0)
	{
		// No recoil crosshair selected in menu.
	}

	else if (Menu::Window.VisualsTab.OtherRecoilCrosshair.GetIndex() == 1)
	{
		// Recoil Crosshair 1

		IClientEntity *pLocal = hackManager.pLocal();

		// Get the view with the recoil
		Vector ViewAngles;
		Interfaces::Engine->GetViewAngles(ViewAngles);
		ViewAngles += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;

		// Build a ray going fowards at that angle
		Vector fowardVec;
		AngleVectors(ViewAngles, &fowardVec);
		fowardVec *= 10000;

		// Get ray start / end
		Vector start = pLocal->GetOrigin() + pLocal->GetViewOffset();
		Vector end = start + fowardVec, endScreen;

		if (Render::WorldToScreen(end, endScreen) && pLocal->IsAlive())
		{
			Render::Line(endScreen.x - 4, endScreen.y - 4, endScreen.x + 4, endScreen.y + 4, Color(0, 255, 0, 255));
			Render::Line(endScreen.x + 4, endScreen.y - 4, endScreen.x - 4, endScreen.y + 4, Color(0, 255, 0, 255));
			Render::Line(endScreen.x - 2, endScreen.y - 2, endScreen.x + 2, endScreen.y + 2, Color(0, 0, 0, 200));
			Render::Line(endScreen.x + 2, endScreen.y - 2, endScreen.x - 2, endScreen.y + 2, Color(0, 0, 0, 200));
		}
	}
}

void CVisuals::NoScopeBorder()
{
	if (Menu::Window.VisualsTab.OtherNoScope.GetState())
	{
		if (hackManager.pLocal()->IsScoped() && hackManager.pLocal()->IsAlive())
		{
			int Width;
			int Height;
			Interfaces::Engine->GetScreenSize(Width, Height);


			int Red = Menu::Window.GUITab.NoScopeR.GetValue();
			int Green = Menu::Window.GUITab.NoScopeG.GetValue();
			int Blue = Menu::Window.GUITab.NoScopeB.GetValue();


			Color cColor = Color(Red, Green, Blue, 255);
			Render::Line(Width / 2, 0, Width / 2, Height, cColor);
			Render::Line(0, Height / 2, Width, Height / 2, cColor);


		}
		else
		{

		}
	}
}

/*
void NightMode()
{
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	ConVar* r_drawspecificstaticprop = Interfaces::CVar->FindVar("r_drawspecificstaticprop");
	SpoofedConvar* r_drawspecificstaticprop_s = new SpoofedConvar(r_drawspecificstaticprop);
	r_drawspecificstaticprop_s->SetInt(0);

	static auto sv_skyname = Interfaces::CVar->FindVar("sv_skyname");
	sv_skyname->nFlags &= ~(1 << 14);

	for (MaterialHandle_t i = Interfaces::MaterialSystem->FirstMaterial(); i != Interfaces::MaterialSystem->InvalidMaterial(); i = Interfaces::MaterialSystem->NextMaterial(i))
	{
		IMaterial* pMaterial = Interfaces::MaterialSystem->GetMaterial(i);

		if (!pMaterial)
			continue;
		if (Menu::Window.VisualsTab.NightMode.GetState())
		{

			if (strstr(pMaterial->GetTextureGroupName(), "Model")) {
				pMaterial->ColorModulate(0.60, 0.60, 0.60);
			}

			if (strstr(pMaterial->GetTextureGroupName(), "World"))
			{
				sv_skyname->SetValue("sky_csgo_night02");
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, false);
				pMaterial->ColorModulate(0.1, 0.1, 0.1);
			}

			if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				pMaterial->ColorModulate(0.3, 0.3, 0.3);
			}

		}
		if (!Menu::Window.VisualsTab.NightMode.GetState())
		{
			if (strstr(pMaterial->GetTextureGroupName(), "Model")) {
				pMaterial->ColorModulate(1, 1, 1);
			}
			if ((strstr(pMaterial->GetTextureGroupName(), "World")) || strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				sv_skyname->SetValue("sky_csgo_night02");
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, false);
				pMaterial->ColorModulate(1, 1, 1);
			}
		}
	}
}; ﻿*/

/*void CVisuals::NightMode()
{
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	ConVar* r_drawspecificstaticprop = Interfaces::CVar->FindVar("r_drawspecificstaticprop");
	SpoofedConvar* r_drawspecificstaticprop_s = new SpoofedConvar(r_drawspecificstaticprop);
	r_drawspecificstaticprop_s->SetInt(0);

	static auto sv_skyname = Interfaces::CVar->FindVar("sv_skyname");
	sv_skyname->nFlags &= ~(1 << 14);

	for (MaterialHandle_t i = Interfaces::MaterialSystem->FirstMaterial(); i != Interfaces::MaterialSystem->InvalidMaterial(); i = Interfaces::MaterialSystem->NextMaterial(i))
	{
		IMaterial* pMaterial = Interfaces::MaterialSystem->GetMaterial(i);

		if (!pMaterial)
			continue;
		if (Menu::Window.VisualsTab.OtherWorld.GetIndex() == 1)
		{

			if (strstr(pMaterial->GetTextureGroupName(), "Model")) {
				pMaterial->ColorModulate(0.60, 0.60, 0.60);
			}

			if (strstr(pMaterial->GetTextureGroupName(), "World"))
			{
				sv_skyname->SetValue("sky_csgo_night02");
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, false);
				pMaterial->ColorModulate(0.30, 0.30, 0.30);
			}

			if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				pMaterial->ColorModulate(0.6, 0.6, 0.6);
			}

		}
		if (Menu::Window.VisualsTab.OtherWorld.GetIndex() == 2)
		{

			if (strstr(pMaterial->GetTextureGroupName(), "Model")) {
				pMaterial->ColorModulate(0.60, 0.60, 0.60);
			}

			if (strstr(pMaterial->GetTextureGroupName(), "World"))
			{
				sv_skyname->SetValue("sky_csgo_night02");
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, false);
				pMaterial->ColorModulate(0.10, 0.10, 0.10);
			}

			if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				pMaterial->ColorModulate(0.3, 0.3, 0.3);
			}

		}
		if (!Menu::Window.VisualsTab.OtherWorld.GetIndex() == 0)
		{
			if (strstr(pMaterial->GetTextureGroupName(), "Model")) {
				pMaterial->ColorModulate(1, 1, 1);
			}
			if ((strstr(pMaterial->GetTextureGroupName(), "World")) || strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				sv_skyname->SetValue("sky_csgo_night02");
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, false);
				pMaterial->ColorModulate(1, 1, 1);
			}
		}
	}
};*/