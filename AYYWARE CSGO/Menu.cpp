#include "Menu.h"
#include "Controls.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "CRC32.h"
#include "RenderManager.h"
// Menu Window Size
#define WINDOW_WIDTH 1065
#define WINDOW_HEIGHT 600
#define charenc( s ) ( s )
AyyWareWindow Menu::Window;

// Save Config to CSGO directory
void SaveCallbk()
{
	switch (Menu::Window.ConfigBox.GetIndex())
	{
	case 0:
		GUI.SaveWindowState(&Menu::Window, "SCCLegit1.cfg");
		break;
	case 1:
		GUI.SaveWindowState(&Menu::Window, "SCCLegit2.cfg");
		break;
	case 2:
		GUI.SaveWindowState(&Menu::Window, "SCCRage1.cfg");
		break;
	case 3:
		GUI.SaveWindowState(&Menu::Window, "SCCRage2.cfg");
		break;
	case 4:
		GUI.SaveWindowState(&Menu::Window, "SCCHvH1.cfg");
		break;
	case 5:
		GUI.SaveWindowState(&Menu::Window, "SCCHvH2.cfg");
		break;
	case 6:
		GUI.SaveWindowState(&Menu::Window, "SCCOther.cfg");
		break;
	}
}









// Load Config from CSGO directory
void LoadCallbk()
{
	switch (Menu::Window.ConfigBox.GetIndex())
	{
	case 0:
		GUI.LoadWindowState(&Menu::Window, "SCCLegit1.cfg");
		break;
	case 1:
		GUI.LoadWindowState(&Menu::Window, "SCCLegit2.cfg");
		break;
	case 2:
		GUI.LoadWindowState(&Menu::Window, "SCCRage1.cfg");
		break;
	case 3:
		GUI.LoadWindowState(&Menu::Window, "SCCRage2.cfg");
		break;
	case 4:
		GUI.LoadWindowState(&Menu::Window, "SCCHvH1.cfg");
		break;
	case 5:
		GUI.LoadWindowState(&Menu::Window, "SCCHvH2.cfg");
		break;
	case 6:
		GUI.LoadWindowState(&Menu::Window, "SCCOther.cfg");
		break;
	}
}

// Unload Function
void UnLoadCallbk()
{
	DoUnload = true;
}

void KnifeApplyCallbk()
{

	
	// Big Meme
	static ConVar* Dank = Interfaces::CVar->FindVar("cl_fullupdate");
	Dank->nFlags &= ~FCVAR_CHEAT;
	Interfaces::Engine->ClientCmd_Unrestricted("cl_fullupdate");



	/* static ConVar* Meme = Interfaces::CVar->FindVar("cl_fullupdate");
	Meme->nFlags &= ~FCVAR_CHEAT;
	Interfaces::Engine->ClientCmd_Unrestricted("cl_fullupdate"); */
}

void AyyWareWindow::Setup()
{
	SetPosition(50, 50);
	SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	
	RegisterTab(&RageBotTab);
	RegisterTab(&LegitBotTab);
	RegisterTab(&VisualsTab);
	RegisterTab(&SkinTab);
	RegisterTab(&MiscTab);
	RegisterTab(&GUITab);

	RECT Client = GetClientArea();
	Client.bottom -= 29;

	RageBotTab.Setup();
	LegitBotTab.Setup();
	VisualsTab.Setup();
	SkinTab.Setup();
	MiscTab.Setup();
	GUITab.Setup();

#pragma region Bottom Buttons

	ConfigBox.SetFileId("cfg_box");
	ConfigBox.AddItem("Legit 1");
	ConfigBox.AddItem("Legit 2");
	ConfigBox.AddItem("Rage 1");
	ConfigBox.AddItem("Rage 2");
	ConfigBox.AddItem("HvH 1");
	ConfigBox.AddItem("HvH 2");
	ConfigBox.AddItem("Other");
	ConfigBox.SetSize(112, 350);
	ConfigBox.SetPosition(600, Client.bottom - 480);

	SaveButton.SetText("Save");
	SaveButton.SetCallback(SaveCallbk);
	SaveButton.SetSize(112, 350);
	SaveButton.SetPosition(600, Client.bottom - 376);

	LoadButton.SetText("Load");
	LoadButton.SetCallback(LoadCallbk);
	LoadButton.SetSize(112, 350);
	LoadButton.SetPosition(600, Client.bottom - 410);

	UnloadButton.SetText("Unload");
	UnloadButton.SetCallback(UnLoadCallbk);
	UnloadButton.SetSize(112, 389);
	UnloadButton.SetPosition(600, Client.bottom - 449);






	MiscTab.RegisterControl(&SaveButton);
	MiscTab.RegisterControl(&LoadButton);
	MiscTab.RegisterControl(&UnloadButton);

	MiscTab.RegisterControl(&ConfigBox);

#pragma endregion Setting up the settings buttons
}

void CRageBotTab::Setup()
{
	SetTitle("A");

	ActiveLabel.SetPosition(10, 8);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(55, 8);
	RegisterControl(&Active);

#pragma region Aimbot


	EzText.SetPosition(500, 30);
	EzText.SetText("SUICIDE.CC");
	//EzText.SetSize(10, 10);
	RegisterControl(&EzText);

	AimbotGroup.SetPosition(10, 55);
	AimbotGroup.SetText("Aimbot");
	
	AimbotGroup.SetSize(240, 490);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl("Enable", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceLabledControl("Auto Fire", this, &AimbotAutoFire);

	AimbotFov.SetFileId("aim_fov");
	AimbotFov.SetBoundaries(0.f, 180.f);
	AimbotFov.SetValue(39.f);
	AimbotGroup.PlaceLabledControl("FOV Range", this, &AimbotFov);

	AimbotSlientSelection.SetFileId("tgt_slientselection");
	AimbotSlientSelection.AddItem("Disabled");
	AimbotSlientSelection.AddItem("Client Side");
	AimbotSlientSelection.AddItem("Server Side");
	AimbotGroup.PlaceLabledControl("Slient Aim", this, &AimbotSlientSelection);

	AimbotAutoPistol.SetFileId("aim_autopistol");
	AimbotGroup.PlaceLabledControl("Auto Pistol", this, &AimbotAutoPistol);

	AimbotAutoRevolver.SetFileId("aim_autorevolver");
	AimbotGroup.PlaceLabledControl("Auto Revolver", this, &AimbotAutoRevolver);
	AutoAwpBody.SetFileId("aim_awpbody");
	AimbotGroup.PlaceLabledControl("Auto body AWP", this, &AutoAwpBody);
	AimbotAimStep.SetFileId("aim_aimstep");
	AimbotGroup.PlaceLabledControl("Aim Step", this, &AimbotAimStep);

	AimbotKeyPress.SetFileId("aim_usekey");
	AimbotGroup.PlaceLabledControl("On Key Press", this, &AimbotKeyPress);

	AimbotKeyBind.SetFileId("aim_key");
	AimbotGroup.PlaceLabledControl("Key", this, &AimbotKeyBind);

	AimbotStopKey.SetFileId("aim_stop");
	AimbotGroup.PlaceLabledControl("Stop Aim", this, &AimbotStopKey);
#pragma endregion Aimbot Controls Get Setup in here

#pragma region Target
	TargetGroup.SetPosition(800, 55);
	TargetGroup.SetText("Target");
	TargetGroup.SetSize(240, 490);
	RegisterControl(&TargetGroup);

	TargetSelection.SetFileId("tgt_selection");
	TargetSelection.AddItem("Closest To Crosshair");
	TargetSelection.AddItem("Distance");
	TargetSelection.AddItem("Lowest Health");
	TargetGroup.PlaceLabledControl("Selection", this, &TargetSelection);

	TargetFriendlyFire.SetFileId("tgt_friendlyfire");
	TargetGroup.PlaceLabledControl("Friendly Fire", this, &TargetFriendlyFire);

	TargetHitbox.SetFileId("tgt_hitbox");
	TargetHitbox.AddItem("Head");
	TargetHitbox.AddItem("Neck");
	TargetHitbox.AddItem("Chest");
	TargetHitbox.AddItem("Stomach");
	TargetHitbox.AddItem("Pelvis");
	TargetGroup.PlaceLabledControl("Hitbox", this, &TargetHitbox);

/*	TargetHitscan.SetFileId("tgt_hitscan");
	TargetHitscan.AddItem("Off"); // Hitscan Case 0
	TargetHitscan.AddItem("Head"); // Hitscan case 1
	TargetHitscan.AddItem("Upperbody"); // Hitscan Case 2
	TargetHitscan.AddItem("Everything"); // Hitscan Case 3
	TargetGroup.PlaceLabledControl("Hitscan", this, &TargetHitscan);*/

	TargetMultipoint.SetFileId("tgt_multipoint");
	TargetGroup.PlaceLabledControl("Multipoint", this, &TargetMultipoint);

	PreferBodyAim.SetFileId("acc_bodyaim");
	PreferBodyAim.AddItem("Off");
	PreferBodyAim.AddItem("Smart");
	PreferBodyAim.AddItem("Baim Only");
	TargetGroup.PlaceLabledControl("Prefer Baim", this, &PreferBodyAim);

	TargetPointscale.SetFileId("tgt_pointscale"); // Pointscale Meme
	TargetPointscale.SetBoundaries(0.f, 100.f);
	TargetPointscale.SetValue(0.f);
	TargetGroup.PlaceOtherControl("Point Scale", this, &TargetPointscale);
#pragma endregion Targetting controls 





	HitScanMenu.SetPosition(800, 250);
	HitScanMenu.SetText("HitScanSettings");
	HitScanMenu.SetSize(240, 45);
	RegisterControl(&HitScanMenu);




	TargetHitscan.SetFileId("tgt_hitscan");
	TargetHitscan.AddItem("off");
	TargetHitscan.AddItem("Low");
	TargetHitscan.AddItem("Medium");
	TargetHitscan.AddItem("Max");

	
	HitScanMenu.PlaceLabledControl("Hitscan", this, &TargetHitscan);


	ResolverMenu.SetPosition(800, 315);
	ResolverMenu.SetText("Resolver Pitch Custom");
	ResolverMenu.SetSize(240, 145);
	RegisterControl(&ResolverMenu);


	Shot1.SetFileId("tgt_shoot1"); // Pointscale Meme
	Shot1.SetBoundaries(0.f, 90.f);
	Shot1.SetValue(0.f);
	ResolverMenu.PlaceOtherControl("Shoot 1 -", this, &Shot1);
	Shot2.SetFileId("tgt_shoot2"); // Pointscale Meme
	Shot2.SetBoundaries(0.f, 90.f);
	Shot2.SetValue(10.f);
	ResolverMenu.PlaceOtherControl("Shoot 2 -", this, &Shot2);
	Shot3.SetFileId("tgt_shoot3"); // Pointscale Meme
	Shot3.SetBoundaries(0.f, 90.f);
	Shot3.SetValue(20.f);
	ResolverMenu.PlaceOtherControl("Shoot 3 +", this, &Shot3);
	Shot4.SetFileId("tgt_shoot4"); // Pointscale Meme
	Shot4.SetBoundaries(0.f, 90.f);
	Shot4.SetValue(20.f);
	ResolverMenu.PlaceOtherControl("Shoot 4 -", this, &Shot4);
	Shot5.SetFileId("tgt_shoot5"); // Pointscale Meme
	Shot5.SetBoundaries(0.f, 90.f);
	Shot5.SetValue(10.f);
	ResolverMenu.PlaceOtherControl("Shoot 5 +", this, &Shot5);

#pragma region Accuracy

	AccuracyGroup.SetPosition(260, 55);
	AccuracyGroup.SetText("Accuracy");
	AccuracyGroup.SetSize(260, 490);
	RegisterControl(&AccuracyGroup);

	AccuracyRecoil.SetFileId("acc_norecoil");
	AccuracyGroup.PlaceLabledControl("Remove Recoil", this, &AccuracyRecoil);

	AccuracyAutoWall.SetFileId("acc_awall");
	AccuracyGroup.PlaceLabledControl("Auto Wall", this, &AccuracyAutoWall);

	AccuracyMinimumDamage.SetFileId("acc_mindmg");
	AccuracyMinimumDamage.SetBoundaries(0.f, 99.f);
	AccuracyMinimumDamage.SetValue(0.f);
	AccuracyGroup.PlaceLabledControl("Autowall Damage", this, &AccuracyMinimumDamage);

	AccuracyAutoStop.SetFileId("acc_stop");
	AccuracyGroup.PlaceLabledControl("Auto Stop / Crouch", this, &AccuracyAutoStop);

	AccuracyAutoScope.SetFileId("acc_scope");
	AccuracyGroup.PlaceLabledControl("Auto Scope", this, &AccuracyAutoScope);

	AccuracyHitchance.SetFileId("acc_chance");
	AccuracyHitchance.SetBoundaries(0, 100);
	AccuracyHitchance.SetValue(0);
	AccuracyGroup.PlaceLabledControl("Hit Chance", this, &AccuracyHitchance);

	PerfectAccuracy.SetFileId("acc_paccuracy");
	AccuracyGroup.PlaceLabledControl("Velocity Adjustment", this, &PerfectAccuracy);

	AccuracyPrediction.SetFileId("acc_prediction");
	AccuracyGroup.PlaceLabledControl("Engine Prediction", this, &AccuracyPrediction);
	
	AccuracyResolverPitch.SetFileId("acc_pitchresolver");
	AccuracyResolverPitch.AddItem("Off");
	AccuracyResolverPitch.AddItem("Snake");
	AccuracyGroup.PlaceLabledControl("Resolver Pitch", this, &AccuracyResolverPitch);

	AccuracyResolverYaw.SetFileId("acc_yawresolver");
	AccuracyResolverYaw.AddItem("Off");
	AccuracyResolverYaw.AddItem("LowerBody Inversive");
	AccuracyResolverYaw.AddItem("Angle Inversive");
	AccuracyResolverYaw.AddItem("Auto Resolver");
	AccuracyResolverYaw.AddItem("Delta Math");
	AccuracyResolverYaw.AddItem("Delta + Old Delta");
	AccuracyResolverYaw.AddItem("Delta Advanced");
	AccuracyResolverYaw.AddItem("Delta Force Method");
	//AccuracyResolverYaw.AddItem("Auto Resolver");
	AccuracyGroup.PlaceLabledControl("Resolver Yaw", this, &AccuracyResolverYaw);
	AccuracyResolverAuto.SetFileId("acc_autoresolver");
	AccuracyResolverAuto.AddItem("Off");
	AccuracyResolverAuto.AddItem("Engine Auto");
	AccuracyGroup.PlaceLabledControl("Auto Resolver", this, &AccuracyResolverAuto);
	PVSFix.SetFileId("acc_pvsfix");
	AccuracyGroup.PlaceLabledControl("PVS Fix", this, &PVSFix);

	FakeLagFix.SetFileId("acc_FakeLagFix");
	AccuracyGroup.PlaceLabledControl("Fake-Lag Fix", this, &FakeLagFix);

	AccuracySmart.SetFileId("acc_smart");
	AccuracySmart.SetBoundaries(0, 20);
	AccuracySmart.SetValue(0);
	AccuracyGroup.PlaceLabledControl("Baim after X Shots", this, &AccuracySmart);

#pragma endregion  Accuracy controls get Setup in here

#pragma region Anti-Aim

	// Anti-Aim
	AntiAimGroup.SetPosition(530, 55);
	AntiAimGroup.SetText("Anti-Aim");
	AntiAimGroup.SetSize(260, 490);
	RegisterControl(&AntiAimGroup);

	AntiAimEnable.SetFileId("aa_enable");
	AntiAimGroup.PlaceLabledControl("Enable", this, &AntiAimEnable);

	// Antiaim List for Pitch
	AntiAimPitch.SetFileId("aa_x");
	AntiAimPitch.AddItem("Disabled");
	AntiAimPitch.AddItem("Emotion");
	AntiAimPitch.AddItem("Down");
	AntiAimPitch.AddItem("Up");
	AntiAimPitch.AddItem("Fake Down");
	AntiAimPitch.AddItem("Fake Up");
	AntiAimPitch.AddItem("Zero");
	AntiAimPitch.AddItem("Fake Zero");
	AntiAimPitch.AddItem("Mixed");
	AntiAimGroup.PlaceLabledControl("Pitch", this, &AntiAimPitch);

	// Antiaim List for Yaw
	AntiAimYaw.SetFileId("aa_y");
	AntiAimYaw.AddItem("Disabled");
	AntiAimYaw.AddItem("Backward");
	AntiAimYaw.AddItem("Static");
	AntiAimYaw.AddItem("Static 2");
	AntiAimYaw.AddItem("Static 3");
	AntiAimYaw.AddItem("Spinbot");
	AntiAimYaw.AddItem("Twitch");
	AntiAimYaw.AddItem("Lower Body");
	AntiAimYaw.AddItem("LBY Breaker");
	AntiAimYaw.AddItem("FlipAA");
	AntiAimYaw.AddItem("KiduaFlip");
	AntiAimYaw.AddItem("LispAA");
	AntiAimYaw.AddItem("FakeSpin");
	AntiAimYaw.AddItem("Fake3");
	AntiAimGroup.PlaceLabledControl("Real Yaw", this, &AntiAimYaw);

	AntiAimYawFake.SetFileId("aa_fy");
	AntiAimYawFake.AddItem("Disabled");
	AntiAimYawFake.AddItem("Backward");
	AntiAimYawFake.AddItem("Spinbot");
	AntiAimYawFake.AddItem("Fake Lower Body");
	AntiAimYawFake.AddItem("LBY Breaker");
	AntiAimYawFake.AddItem("FlipAA");
	AntiAimYawFake.AddItem("KiduaFlip");
	AntiAimYawFake.AddItem("LispAA");
	AntiAimYawFake.AddItem("FakeSpin");
	AntiAimYawFake.AddItem("Fake3");
	AntiAimGroup.PlaceLabledControl("Fake Yaw", this, &AntiAimYawFake);


	AntiAimReady.SetFileId("aa_r");
	AntiAimReady.AddItem("Use Real + Fake");
	AntiAimReady.AddItem("Fake Jitter Kidua");
	AntiAimReady.AddItem("LowerBody Jitter");
	AntiAimReady.AddItem("Backwards");
	AntiAimReady.AddItem("Fake Backwards");
	AntiAimReady.AddItem("Flip LBY 135");
	AntiAimReady.AddItem("Fake Sideways");
	AntiAimReady.AddItem("Custom LBY Flip");
	AntiAimReady.AddItem("FakeSpin");
	//AntiAimReady.AddItem("LBY Breaker");
	AntiAimGroup.PlaceLabledControl("Ready Anti Aims", this, &AntiAimReady);


	MinusLBYADD.SetFileId("aa_minuslby");
	AntiAimGroup.PlaceLabledControl("MINUS LBY ADD FLIP YAW", this, &MinusLBYADD);
	AddLBYYAWFLIP.SetFileId("add_lbyflip");
	AddLBYYAWFLIP.SetBoundaries(0.f, 180.f);
	AddLBYYAWFLIP.SetValue(0.f);
	AntiAimGroup.PlaceLabledControl("Add LBY FLIP YAW", this, &AddLBYYAWFLIP);


	AntiAimJitterLBY.SetFileId("aa_jitterlby");
	AntiAimGroup.PlaceLabledControl("Jitter LBY", this, &AntiAimJitterLBY);



	AntiAimSpinspeed.SetFileId("aa_spinspeed");
	AntiAimSpinspeed.SetBoundaries(0, 20);
	AntiAimSpinspeed.SetValue(0);
	AntiAimGroup.PlaceLabledControl("Spinbot Speed", this, &AntiAimSpinspeed);

	AntiAimKnife.SetFileId("aa_knife");
	AntiAimGroup.PlaceLabledControl("Anti Aim on Knife", this, &AntiAimKnife);

	AntiAimTarget.SetFileId("aa_target");
	AntiAimGroup.PlaceLabledControl("Anti Aim At Target", this, &AntiAimTarget);

	AntiAimEdge.SetFileId("aa_edge");
	AntiAimGroup.PlaceLabledControl("Wall DTC", this, &AntiAimEdge);

	AntiAimFakeWalk.SetFileId("aa_edge");
	AntiAimGroup.PlaceLabledControl("Fake Walk", this, &AntiAimFakeWalk);
	// AntiAimEdge.SetFileId("aa_edge");
	// AntiAimGroup.PlaceLabledControl("Anti Aim Edge", this, &AntiAimEdge);

#pragma endregion  AntiAim controls get setup in here
}

void CLegitBotTab::Setup()
{
	SetTitle("I");


	EzTextLegit.SetPosition(500, 30);
	EzTextLegit.SetText("SUICIDE.CC");
	//EzText.SetSize(10, 10);
	RegisterControl(&EzTextLegit);


	ActiveLabel.SetPosition(10, 38);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(55, 8);
	RegisterControl(&Active);

#pragma region Aimbot
	AimbotGroup.SetPosition(10, 65);
	AimbotGroup.SetText("Aimbot");
	AimbotGroup.SetSize(240, 210);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl("Enable", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceLabledControl("Auto Fire", this, &AimbotAutoFire);

	AimbotFriendlyFire.SetFileId("aim_friendfire");
	AimbotGroup.PlaceLabledControl("Friendly Fire", this, &AimbotFriendlyFire);

	AimbotKeyPress.SetFileId("aim_usekey");
	AimbotGroup.PlaceLabledControl("On Key Press", this, &AimbotKeyPress);

	AimbotKeyBind.SetFileId("aim_key");
	AimbotGroup.PlaceLabledControl("Key Bind", this, &AimbotKeyBind);
	
	AimbotAutoPistol.SetFileId("aim_apistol");
	AimbotGroup.PlaceLabledControl("Auto Pistol", this, &AimbotAutoPistol);

#pragma endregion Aimbot shit

#pragma region Triggerbot

	TriggerGroup.SetPosition(272, 65);
	TriggerGroup.SetText("Triggerbot");
	TriggerGroup.SetSize(240, 210);
	RegisterControl(&TriggerGroup);

	TriggerEnable.SetFileId("trig_enable");
	TriggerGroup.PlaceLabledControl("Enable", this, &TriggerEnable);

	TriggerKeyPress.SetFileId("trig_onkey");
	TriggerGroup.PlaceLabledControl("On Key Press", this, &TriggerKeyPress);

	TriggerKeyBind.SetFileId("trig_key");
	TriggerGroup.PlaceLabledControl("Key Bind", this, &TriggerKeyBind);

	TriggerDelay.SetFileId("trig_time");
	TriggerDelay.SetBoundaries(0.f, 1000.f);
	TriggerGroup.PlaceLabledControl("Delay (ms)", this, &TriggerDelay);

#pragma endregion Triggerbot stuff

#pragma region Main Weapon

	WeaponMainGroup.SetPosition(10, 290);
	WeaponMainGroup.SetText("Rifles/Other");
	WeaponMainGroup.SetSize(210, 210);
	RegisterControl(&WeaponMainGroup);

	WeaponMainSpeed.SetFileId("main_speed");
	WeaponMainSpeed.SetBoundaries(0.1f, 2.f);
	WeaponMainSpeed.SetValue(1.0f);
	WeaponMainGroup.PlaceLabledControl("Max Speed", this, &WeaponMainSpeed);

	WeaponMainFoV.SetFileId("main_fov");
	WeaponMainFoV.SetBoundaries(0.1f, 30.f);
	WeaponMainFoV.SetValue(5.f);
	WeaponMainGroup.PlaceLabledControl("FoV", this, &WeaponMainFoV);

	WeaponMainRecoil.SetFileId("main_recoil");
	WeaponMainGroup.PlaceLabledControl("Recoil", this, &WeaponMainRecoil);

	WeaponMainPSilent.SetFileId("main_psilent");
	WeaponMainGroup.PlaceLabledControl("Perfect Silent", this, &WeaponMainPSilent);

	WeaponMainInacc.SetFileId("main_inacc");
	WeaponMainInacc.SetBoundaries(0.f, 15.f);
	WeaponMainGroup.PlaceLabledControl("Inaccuracy", this, &WeaponMainInacc);

	WeaponMainHitbox.SetFileId("main_hitbox");
	WeaponMainHitbox.AddItem("Head");
	WeaponMainHitbox.AddItem("Neck");
	WeaponMainHitbox.AddItem("Chest");
	WeaponMainHitbox.AddItem("Stomach");
	WeaponMainGroup.PlaceLabledControl("Hitbox", this, &WeaponMainHitbox);

#pragma endregion

#pragma region Pistols
	WeaponPistGroup.SetPosition(230, 290);
	WeaponPistGroup.SetText("Pistols");
	WeaponPistGroup.SetSize(210, 210);
	RegisterControl(&WeaponPistGroup);

	WeaponPistSpeed.SetFileId("pist_speed");
	WeaponPistSpeed.SetBoundaries(0.1f, 2.f);
	WeaponPistSpeed.SetValue(1.0f);
	WeaponPistGroup.PlaceLabledControl("Max Speed", this, &WeaponPistSpeed);

	WeaponPistFoV.SetFileId("pist_fov");
	WeaponPistFoV.SetBoundaries(0.1f, 30.f);
	WeaponPistFoV.SetValue(5.f);
	WeaponPistGroup.PlaceLabledControl("FoV", this, &WeaponPistFoV);

	WeaponPistRecoil.SetFileId("pist_recoil");
	WeaponPistGroup.PlaceLabledControl("Recoil", this, &WeaponPistRecoil);

	WeaponPistPSilent.SetFileId("pist_psilent");
	WeaponPistGroup.PlaceLabledControl("Perfect Silent", this, &WeaponPistPSilent);

	WeaponPistInacc.SetFileId("pist_inacc");
	WeaponPistInacc.SetBoundaries(0.f, 15.f);
	WeaponPistGroup.PlaceLabledControl("Inaccuracy", this, &WeaponPistInacc);

	WeaponPistHitbox.SetFileId("pist_hitbox");
	WeaponPistHitbox.AddItem("Head");
	WeaponPistHitbox.AddItem("Neck");
	WeaponPistHitbox.AddItem("Chest");
	WeaponPistHitbox.AddItem("Stomach");
	WeaponPistGroup.PlaceLabledControl("Hitbox", this, &WeaponPistHitbox);
#pragma endregion

#pragma region Snipers
	WeaponSnipGroup.SetPosition(450, 290);
	WeaponSnipGroup.SetText("Snipers");
	WeaponSnipGroup.SetSize(210, 210);
	RegisterControl(&WeaponSnipGroup);

	WeaponSnipSpeed.SetFileId("snip_speed");
	WeaponSnipSpeed.SetBoundaries(0.1f, 2.f);
	WeaponSnipSpeed.SetValue(1.0f);
	WeaponSnipGroup.PlaceLabledControl("Max Speed", this, &WeaponSnipSpeed);

	WeaponSnipFoV.SetFileId("snip_fov");
	WeaponSnipFoV.SetBoundaries(0.1f, 30.f);
	WeaponSnipFoV.SetValue(5.f);
	WeaponSnipGroup.PlaceLabledControl("FoV", this, &WeaponSnipFoV);

	WeaponSnipRecoil.SetFileId("snip_recoil");
	WeaponSnipGroup.PlaceLabledControl("Recoil", this, &WeaponSnipRecoil);

	WeaponSnipPSilent.SetFileId("snip_psilent");
	WeaponSnipGroup.PlaceLabledControl("Perfect Silent", this, &WeaponSnipPSilent);

	WeaponSnipInacc.SetFileId("snip_inacc");
	WeaponSnipInacc.SetBoundaries(0.f, 15.f);
	WeaponSnipGroup.PlaceLabledControl("Inaccuracy", this, &WeaponSnipInacc);

	WeaponSnipHitbox.SetFileId("snip_hitbox");
	WeaponSnipHitbox.AddItem("Head");
	WeaponSnipHitbox.AddItem("Neck");
	WeaponSnipHitbox.AddItem("Chest");
	WeaponSnipHitbox.AddItem("Stomach");
	WeaponSnipGroup.PlaceLabledControl("Hitbox", this, &WeaponSnipHitbox);
#pragma endregion
}

void CVisualTab::Setup()
{
	SetTitle("D");


	EzTextVisual.SetPosition(500, 30);
	EzTextVisual.SetText("SUICIDE.CC");
	//EzText.SetSize(10, 10);
	RegisterControl(&EzTextVisual);

	ActiveLabel.SetPosition(10, 8);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(55, 8);
	RegisterControl(&Active);

#pragma region Options
	OptionsGroup.SetText("Options");
	OptionsGroup.SetPosition(10, 55);
	OptionsGroup.SetSize(185, 290);
	RegisterControl(&OptionsGroup);

	OptionsBox.SetFileId("opt_box");
	OptionsGroup.PlaceLabledControl("Box", this, &OptionsBox);

	BoxDesign.SetFileId("opt_boxtype");
	BoxDesign.AddItem("Corners");
	BoxDesign.AddItem("Whole");
	OptionsGroup.PlaceLabledControl("Box Type", this, &BoxDesign);

	OptionsName.SetFileId("opt_name");
	OptionsGroup.PlaceLabledControl("Name", this, &OptionsName);

	OptionHealthEnable.SetFileId("opt_health");
	OptionsGroup.PlaceLabledControl("Health", this, &OptionHealthEnable);

	OptionsHealth.SetFileId("opt_hp");
	OptionsHealth.AddItem("Interwebz");
	OptionsHealth.AddItem("Aimware");
	OptionsHealth.AddItem("New Health Bar");
	OptionsGroup.PlaceLabledControl("Health", this, &OptionsHealth);

	OptionsWeapon.SetFileId("opt_weapon");
	OptionsGroup.PlaceLabledControl("Weapon", this, &OptionsWeapon);

	OptionsInfo.SetFileId("opt_info");
	OptionsGroup.PlaceLabledControl("Info", this, &OptionsInfo);

	OptionsChams.SetFileId("opt_chams");
	OptionsChams.AddItem("Off");
	OptionsChams.AddItem("Normal");
	OptionsChams.AddItem("Flat");
	OptionsGroup.PlaceLabledControl("Chams", this, &OptionsChams);

	OptionsSkeleton.SetFileId("opt_bone");
	OptionsGroup.PlaceLabledControl("Skeleton", this, &OptionsSkeleton);
	Glow.SetFileId("opt_glow");
	OptionsGroup.PlaceLabledControl("Glow", this, &Glow);

	GlowAlpha.SetFileId("otp_glowalpha");
	OptionsGroup.PlaceLabledControl("Glow Opacity", this, &GlowAlpha);
	GlowAlpha.SetBoundaries(0.f, 255.f);
	GlowAlpha.SetValue(200.f);



	BulletTrace.SetFileId("otp_bulletrace");
	OptionsGroup.PlaceLabledControl("Bullet Trace", this, &BulletTrace);

	BulletTraceLength.SetFileId("otp_bulletracerange");
	OptionsGroup.PlaceLabledControl("Bullet Trace Range", this, &BulletTraceLength);
	BulletTraceLength.SetBoundaries(0.f, 1000.f);
	BulletTraceLength.SetValue(200.f);


#pragma endregion Setting up the Options controls


	StaffGroup.SetText("Misc Visuals");
	StaffGroup.SetPosition(700, 55);
	StaffGroup.SetSize(250, 350);
	RegisterControl(&StaffGroup);

	OptionsAimSpot.SetFileId("opt_aimspot");
	StaffGroup.PlaceLabledControl("Head Cross", this, &OptionsAimSpot);

	OptionsCompRank.SetFileId("opt_comprank");
	StaffGroup.PlaceLabledControl("Player Ranks", this, &OptionsCompRank);


	OptionsLBY.SetFileId("opt_LBY");
	StaffGroup.PlaceLabledControl("LBY INDICATOR", this, &OptionsLBY);


	IsDefusing.SetFileId("opt_hedufusinglel");
	StaffGroup.PlaceLabledControl("Is Defusing", this, &IsDefusing);

	IsScoped.SetFileId("opt_scoped");
	StaffGroup.PlaceLabledControl("Is Scoped", this, &IsScoped);


	HasDefuser.SetFileId("opt_dontbealoserbuyadefuser");
	StaffGroup.PlaceLabledControl("Has Defuser", this, &HasDefuser);

	OptionsFillBox.SetFileId("opt_fillbox");
	StaffGroup.PlaceLabledControl("Fill Box Enable", this, &OptionsFillBox);

	OtherThirdperson.SetFileId("aa_thirdpsr");
	StaffGroup.PlaceLabledControl("Thirdperson", this, &OtherThirdperson);

	/*OtherThirdpersonRange.SetFileId("otp_ThirdpersonRange");
	OtherThirdpersonRange.SetBoundaries(0.f, 1000.f);
	OtherThirdpersonRange.SetValue(100.f);
	StaffGroup.PlaceLabledControl("Thirdperson Range", this, &OtherThirdpersonRange);*/
	
	OtherThirdpersonAngle.SetFileId("aa_tpAngle");
	OtherThirdpersonAngle.AddItem("Fake Angle");
	OtherThirdpersonAngle.AddItem("Real Angle");
	StaffGroup.PlaceLabledControl("Show Angle", this, &OtherThirdpersonAngle);





/*
	Radar.SetFileId("aa_extRadar");
	StaffGroup.PlaceLabledControl("External Radar", this, &Radar);

	OtherRadarXPosition.SetFileId("otp_radarposx");
	StaffGroup.PlaceLabledControl("Pos X", this, &OtherRadarXPosition);
	OtherRadarXPosition.SetBoundaries(0.f, 2000.f);
	OtherRadarXPosition.SetValue(50.f);
	

	OtherRadarYPosition.SetFileId("otp_radarposy");
	StaffGroup.PlaceLabledControl("Pos Y", this, &OtherRadarYPosition);
	OtherRadarYPosition.SetBoundaries(0.f, 2000.f);
	OtherRadarYPosition.SetValue(50.f);




	OtherRadarEnemyOnly.SetFileId("aa_extRadarEnemyOnly");
	StaffGroup.PlaceLabledControl("Radar Enemy Only", this, &OtherRadarEnemyOnly);

	OtherRadarVisibleOnly.SetFileId("aa_extRadarVisOnly");
	StaffGroup.PlaceLabledControl("Radar Visable Only", this, &OtherRadarVisibleOnly);



	OtherRadarScale.SetFileId("otp_radarscale");
	StaffGroup.PlaceLabledControl("Radar Scale", this, &OtherRadarScale);
	OtherRadarScale.SetBoundaries(0.f, 5000.f);
	OtherRadarScale.SetValue(5000.f);
	*/





#pragma region Filters
	FiltersGroup.SetText("Filters");
	FiltersGroup.SetPosition(205, 55);
	FiltersGroup.SetSize(150, 290);
	RegisterControl(&FiltersGroup);

	FiltersAll.SetFileId("ftr_all");
	FiltersGroup.PlaceLabledControl("All", this, &FiltersAll);

	FiltersPlayers.SetFileId("ftr_players");
	FiltersGroup.PlaceLabledControl("Players", this, &FiltersPlayers);

	FiltersEnemiesOnly.SetFileId("ftr_enemyonly");
	FiltersGroup.PlaceLabledControl("Enemies Only", this, &FiltersEnemiesOnly);

	FiltersWeapons.SetFileId("ftr_weaps");
	FiltersGroup.PlaceLabledControl("Weapons", this, &FiltersWeapons);

	FiltersChickens.SetFileId("ftr_chickens");
	FiltersGroup.PlaceLabledControl("Chickens", this, &FiltersChickens);

	FiltersC4.SetFileId("ftr_c4");
	FiltersGroup.PlaceLabledControl("C4", this, &FiltersC4);
#pragma endregion Setting up the Filters controls






	











#pragma region Other
	OtherGroup.SetText("Other");
	OtherGroup.SetPosition(365, 55);
	OtherGroup.SetSize(290, 290);
	RegisterControl(&OtherGroup);

	OtherCrosshair.SetFileId("otr_xhair");
	OtherCrosshair.AddItem("Off");
	OtherCrosshair.AddItem("Rainbow X");
	OtherCrosshair.AddItem("Rainbow Swastika");
	OtherGroup.PlaceLabledControl("Crosshair", this, &OtherCrosshair);

	OtherRecoilCrosshair.SetFileId("otr_recoilhair");
	OtherRecoilCrosshair.AddItem("Off");
	OtherRecoilCrosshair.AddItem("Recoil Position");
	OtherGroup.PlaceLabledControl("Recoil Crosshair", this, &OtherRecoilCrosshair);

	OtherRadar.SetFileId("otr_radar");
	OtherGroup.PlaceLabledControl("Radar", this, &OtherRadar);

	OtherNoVisualRecoil.SetFileId("otr_visrecoil");
	OtherGroup.PlaceLabledControl("No Visual Recoil", this, &OtherNoVisualRecoil);

	OtherNoFlash.SetFileId("otr_noflash");
	OtherGroup.PlaceLabledControl("No Flash", this, &OtherNoFlash);

	OtherNoHands.SetFileId("otr_hands");
	OtherNoHands.AddItem("Off");
	OtherNoHands.AddItem("None");
	OtherNoHands.AddItem("Transparent");
	OtherNoHands.AddItem("Chams");
	OtherNoHands.AddItem("Rainbow");
	OtherGroup.PlaceLabledControl("Hands", this, &OtherNoHands);


	OtherFOV.SetFileId("otr_fov");
	OtherFOV.SetBoundaries(0.f, 130.f);
	OtherFOV.SetValue(90.f);
	OtherGroup.PlaceLabledControl("FOV", this, &OtherFOV);

	OtherViewmodelFOV.SetFileId("otr_viewfov");
	OtherViewmodelFOV.SetBoundaries(0.f, 20.f);
	OtherViewmodelFOV.SetValue(0.f);
	OtherGroup.PlaceLabledControl("Viewmodel FOV +", this, &OtherViewmodelFOV);

	CustomSky.SetFileId("otr_customsky");
	OtherGroup.PlaceLabledControl("Custom Skies", this, &CustomSky);

	OtherNoScope.SetFileId("otr_noscope");
	OtherGroup.PlaceLabledControl("Noscope", this, &OtherNoScope);



#pragma endregion Setting up the Other controls

#pragma region ESPColorSkeleton
	ESPColorSkeleton.SetText("ESP Color");
	ESPColorSkeleton.SetPosition(10, 385);
	ESPColorSkeleton.SetSize(180, 150);
	RegisterControl(&ESPColorSkeleton);

	RSkele.SetFileId("colormenu_rmma");
	RSkele.SetBoundaries(0, 255);
	RSkele.SetValue(255);
	ESPColorSkeleton.PlaceLabledControl("R", this, &RSkele);

	GSkele.SetFileId("colormenu_gmma");
	GSkele.SetBoundaries(0, 255);
	GSkele.SetValue(0);
	ESPColorSkeleton.PlaceLabledControl("G", this, &GSkele);

	BSkele.SetFileId("colormenu_bmma");
	BSkele.SetBoundaries(0, 255);
	BSkele.SetValue(0);
	ESPColorSkeleton.PlaceLabledControl("B", this, &BSkele);

	ASkele.SetFileId("colormenu_amma");
	ASkele.SetBoundaries(0, 255);
	ASkele.SetValue(255);
	ESPColorSkeleton.PlaceLabledControl("A", this, &ASkele);

#pragma endregion

#pragma region ESPColorBox
	ESPColorBox.SetText("CT ESP");
	ESPColorBox.SetPosition(200, 355);
	ESPColorBox.SetSize(190, 200);
	RegisterControl(&ESPColorBox);

	RBoxCT.SetFileId("colorbox_rmma");
	RBoxCT.SetBoundaries(0, 255);
	RBoxCT.SetValue(15);
	ESPColorBox.PlaceLabledControl("R CT", this, &RBoxCT);

	GBoxCT.SetFileId("colorbox_gmma");
	GBoxCT.SetBoundaries(0, 255);
	GBoxCT.SetValue(110);
	ESPColorBox.PlaceLabledControl("G CT", this, &GBoxCT);

	BBoxCT.SetFileId("colorbox_bmma");
	BBoxCT.SetBoundaries(0, 255);
	BBoxCT.SetValue(220);
	ESPColorBox.PlaceLabledControl("B CT", this, &BBoxCT);

	ABoxCT.SetFileId("colorbox_amma");
	ABoxCT.SetBoundaries(0, 255);
	ABoxCT.SetValue(255);
	ESPColorBox.PlaceLabledControl("A CT", this, &ABoxCT);

	RBoxCTV.SetFileId("colorbox_rmma");
	RBoxCTV.SetBoundaries(0, 255);
	RBoxCTV.SetValue(120);
	ESPColorBox.PlaceLabledControl("R CT Visible", this, &RBoxCTV);

	GBoxCTV.SetFileId("colorbox_gmma");
	GBoxCTV.SetBoundaries(0, 255);
	GBoxCTV.SetValue(210);
	ESPColorBox.PlaceLabledControl("G CT Visible", this, &GBoxCTV);

	BBoxCTV.SetFileId("colorbox_bmma");
	BBoxCTV.SetBoundaries(0, 255);
	BBoxCTV.SetValue(26);
	ESPColorBox.PlaceLabledControl("B CT Visible", this, &BBoxCTV);

	ABoxCTV.SetFileId("colorbox_amma");
	ABoxCTV.SetBoundaries(0, 255);
	ABoxCTV.SetValue(255);
	ESPColorBox.PlaceLabledControl("A CT Visible", this, &ABoxCTV);

#pragma endregion

}

void CSkinTab::Setup()
{
	SetTitle("B");

	EzTextSkins.SetPosition(500, 30);
	EzTextSkins.SetText("SUICIDE.CC");
	//EzText.SetSize(10, 10);
	RegisterControl(&EzTextSkins);

	SkinActive.SetPosition(10, 8);
	SkinActive.SetText("Active");
	RegisterControl(&SkinActive);

	SkinEnable.SetFileId("skin_enable");
	SkinEnable.SetPosition(55, 8);
	RegisterControl(&SkinEnable);

	SkinApply.SetText("Apply");
	SkinApply.SetCallback(KnifeApplyCallbk);
	SkinApply.SetPosition(270, 490);
	SkinApply.SetSize(250, 50);
	RegisterControl(&SkinApply);

#pragma region Knife
	KnifeGroup.SetPosition(10, 55);
	KnifeGroup.SetText("Knife");
	KnifeGroup.SetSize(250, 80);
	RegisterControl(&KnifeGroup);

	KnifeModel.SetFileId("knife_model");
	KnifeModel.AddItem("Bayonet");
	KnifeModel.AddItem("Bowie Knife");
	KnifeModel.AddItem("Butterfly Knife");
	KnifeModel.AddItem("Falchion Knife");
	KnifeModel.AddItem("Flip Knife");
	KnifeModel.AddItem("Gut Knife");
	KnifeModel.AddItem("Huntsman Knife");
	KnifeModel.AddItem("Karambit");
	KnifeModel.AddItem("M9 Bayonet");
	KnifeModel.AddItem("Shadow Daggers");
	KnifeGroup.PlaceLabledControl("Knife", this, &KnifeModel);

	KnifeSkin.SetFileId("knife_skin");
	KnifeSkin.AddItem("None");
	KnifeSkin.AddItem("Crimson Web");
	KnifeSkin.AddItem("Bone Mask");
	KnifeSkin.AddItem("Fade");
	KnifeSkin.AddItem("Night");
	KnifeSkin.AddItem("Blue Steel");
	KnifeSkin.AddItem("Stained");
	KnifeSkin.AddItem("Case Hardened");
	KnifeSkin.AddItem("Slaughter");
	KnifeSkin.AddItem("Safari Mesh");
	KnifeSkin.AddItem("Boreal Forest");
	KnifeSkin.AddItem("Ultraviolet");
	KnifeSkin.AddItem("Urban Masked");
	KnifeSkin.AddItem("Scorched");
	KnifeSkin.AddItem("Rust Coat");
	KnifeSkin.AddItem("Tiger Tooth");
	KnifeSkin.AddItem("Damascus Steel");
	KnifeSkin.AddItem("Damascus Steel");
	KnifeSkin.AddItem("Marble Fade");
	KnifeSkin.AddItem("Rust Coat");
	KnifeSkin.AddItem("Doppler Ruby");
	KnifeSkin.AddItem("Doppler Sapphire");
	KnifeSkin.AddItem("Doppler Blackpearl");
	KnifeSkin.AddItem("Doppler Phase 1");
	KnifeSkin.AddItem("Doppler Phase 2");
	KnifeSkin.AddItem("Doppler Phase 3");
	KnifeSkin.AddItem("Doppler Phase 4");
	KnifeSkin.AddItem("Gamma Doppler Phase 1");
	KnifeSkin.AddItem("Gamma Doppler Phase 2");
	KnifeSkin.AddItem("Gamma Doppler Phase 3");
	KnifeSkin.AddItem("Gamma Doppler Phase 4");
	KnifeSkin.AddItem("Gamma Doppler Emerald");
	KnifeSkin.AddItem("Lore");
	KnifeGroup.PlaceLabledControl("Skin", this, &KnifeSkin);
#pragma endregion

#pragma region Machineguns
	MachinegunsGroup.SetPosition(270, 55);
	MachinegunsGroup.SetText("Machineguns");
	MachinegunsGroup.SetSize(250, 80);
	RegisterControl(&MachinegunsGroup);

	NEGEVSkin.SetFileId("negev_skin");
	NEGEVSkin.AddItem("Anodized Navy");
	NEGEVSkin.AddItem("Man-o'-war");
	NEGEVSkin.AddItem("Palm");
	NEGEVSkin.AddItem("VariCamo");
	NEGEVSkin.AddItem("Palm");
	NEGEVSkin.AddItem("CaliCamo");
	NEGEVSkin.AddItem("Terrain");
	NEGEVSkin.AddItem("Army Sheen");
	NEGEVSkin.AddItem("Bratatat");
	NEGEVSkin.AddItem("Desert-Strike");
	NEGEVSkin.AddItem("Nuclear Waste");
	NEGEVSkin.AddItem("Loudmouth");
	NEGEVSkin.AddItem("Power Loader");
	MachinegunsGroup.PlaceLabledControl("Negev", this, &NEGEVSkin);

	M249Skin.SetFileId("m249_skin");
	M249Skin.AddItem("Contrast Spray");
	M249Skin.AddItem("Blizzard Marbleized");
	M249Skin.AddItem("Jungle DDPAT");
	M249Skin.AddItem("Gator Mesh");
	M249Skin.AddItem("Magma");
	M249Skin.AddItem("System Lock");
	M249Skin.AddItem("Shipping Forecast");
	M249Skin.AddItem("Impact Drill");
	M249Skin.AddItem("Nebula Crusader");
	M249Skin.AddItem("Spectre");
	MachinegunsGroup.PlaceLabledControl("M249", this, &M249Skin);

#pragma endregion

#pragma region Snipers
	Snipergroup.SetPosition(10, 145);
	Snipergroup.SetText("Snipers");
	Snipergroup.SetSize(250, 125);
	RegisterControl(&Snipergroup);

	// AWP
	AWPSkin.SetFileId("awp_skin");
	AWPSkin.AddItem("BOOM");
	AWPSkin.AddItem("Dragon Lore");
	AWPSkin.AddItem("Pink DDPAT");
	AWPSkin.AddItem("Snake Camo");
	AWPSkin.AddItem("Lightning Strike");
	AWPSkin.AddItem("Safari Mesh");
	AWPSkin.AddItem("Corticera");
	AWPSkin.AddItem("Redline");
	AWPSkin.AddItem("Man-o'-war");
	AWPSkin.AddItem("Graphite");
	AWPSkin.AddItem("Electric Hive");
	AWPSkin.AddItem("Pit Viper");
	AWPSkin.AddItem("Asiimov");
	AWPSkin.AddItem("Worm God");
	AWPSkin.AddItem("Medusa");
	AWPSkin.AddItem("Sun in Leo");
	AWPSkin.AddItem("Hyper Beast");
	AWPSkin.AddItem("Elite Build");
	AWPSkin.AddItem("Fever Dream");
	AWPSkin.AddItem("Oni Taiji");
	Snipergroup.PlaceLabledControl("AWP", this, &AWPSkin);

	// SSG08
	SSG08Skin.SetFileId("sgg08_skin");
	SSG08Skin.AddItem("Lichen Dashed");
	SSG08Skin.AddItem("Dark Water");
	SSG08Skin.AddItem("Blue Spruce");
	SSG08Skin.AddItem("Sand Dune");
	SSG08Skin.AddItem("Palm");
	SSG08Skin.AddItem("Mayan Dreams");
	SSG08Skin.AddItem("Blood in the Water");
	SSG08Skin.AddItem("Tropical Storm");
	SSG08Skin.AddItem("Acid Fade");
	SSG08Skin.AddItem("Slashed");
	SSG08Skin.AddItem("Detour");
	SSG08Skin.AddItem("Abyss");
	SSG08Skin.AddItem("Big Iron");
	SSG08Skin.AddItem("Necropos");
	SSG08Skin.AddItem("Ghost Crusader");
	SSG08Skin.AddItem("Dragonfire");
	Snipergroup.PlaceLabledControl("SGG 08", this, &SSG08Skin);

	// SCAR20
	SCAR20Skin.SetFileId("scar20_skin");
	SCAR20Skin.AddItem("Splash Jam");
	SCAR20Skin.AddItem("Storm");
	SCAR20Skin.AddItem("Contractor");
	SCAR20Skin.AddItem("Carbon Fiber");
	SCAR20Skin.AddItem("Sand Mesh");
	SCAR20Skin.AddItem("Palm");
	SCAR20Skin.AddItem("Emerald");
	SCAR20Skin.AddItem("Crimson Web");
	SCAR20Skin.AddItem("Cardiac");
	SCAR20Skin.AddItem("Army Sheen");
	SCAR20Skin.AddItem("Cyrex");
	SCAR20Skin.AddItem("Grotto");
	SCAR20Skin.AddItem("Emerald");
	SCAR20Skin.AddItem("Green Marine");
	SCAR20Skin.AddItem("Outbreak");
	SCAR20Skin.AddItem("Bloodsport");
	Snipergroup.PlaceLabledControl("SCAR-20", this, &SCAR20Skin);

	// G3SG1
	G3SG1Skin.SetFileId("g3sg1_skin");
	G3SG1Skin.AddItem("Desert Storm");
	G3SG1Skin.AddItem("Arctic Camo");
	G3SG1Skin.AddItem("Bone Mask");
	G3SG1Skin.AddItem("Contractor");
	G3SG1Skin.AddItem("Safari Mesh");
	G3SG1Skin.AddItem("Polar Camo");
	G3SG1Skin.AddItem("Jungle Dashed");
	G3SG1Skin.AddItem("VariCamo");
	G3SG1Skin.AddItem("Predator");
	G3SG1Skin.AddItem("Demeter");
	G3SG1Skin.AddItem("Azure Zebra");
	G3SG1Skin.AddItem("Green Apple");
	G3SG1Skin.AddItem("Orange Kimono");
	G3SG1Skin.AddItem("Neon Kimono");
	G3SG1Skin.AddItem("Murky");
	G3SG1Skin.AddItem("Chronos");
	G3SG1Skin.AddItem("Flux");
	G3SG1Skin.AddItem("The Executioner");
	G3SG1Skin.AddItem("Orange Crash");
	Snipergroup.PlaceLabledControl("G3SG1", this, &G3SG1Skin);
#pragma endregion

#pragma region Shotguns
	Shotgungroup.SetPosition(270, 145);
	Shotgungroup.SetText("Shotguns");
	Shotgungroup.SetSize(250, 125);
	RegisterControl(&Shotgungroup);

	MAG7Skin.SetFileId("mag7_skin");
	MAG7Skin.AddItem("Counter Terrace");
	MAG7Skin.AddItem("Metallic DDPAT");
	MAG7Skin.AddItem("Silver");
	MAG7Skin.AddItem("Storm");
	MAG7Skin.AddItem("Bulldozer");
	MAG7Skin.AddItem("Heat");
	MAG7Skin.AddItem("Sand Dune");
	MAG7Skin.AddItem("Irradiated Alert");
	MAG7Skin.AddItem("Memento");
	MAG7Skin.AddItem("Hazard");
	MAG7Skin.AddItem("Heaven Guard");
	MAG7Skin.AddItem("Firestarter");
	MAG7Skin.AddItem("Seabird");
	MAG7Skin.AddItem("Cobalt Core");
	MAG7Skin.AddItem("Praetorian");
	Shotgungroup.PlaceLabledControl("Mag-7", this, &MAG7Skin);

	XM1014Skin.SetFileId("xm1014_skin");
	XM1014Skin.AddItem("Blaze Orange");
	XM1014Skin.AddItem("VariCamo Blue");
	XM1014Skin.AddItem("Bone Mask");
	XM1014Skin.AddItem("Blue Steel");
	XM1014Skin.AddItem("Blue Spruce");
	XM1014Skin.AddItem("Grassland");
	XM1014Skin.AddItem("Urban Perforated");
	XM1014Skin.AddItem("Jungle");
	XM1014Skin.AddItem("VariCamo");
	XM1014Skin.AddItem("VariCamo");
	XM1014Skin.AddItem("Fallout Warning");
	XM1014Skin.AddItem("Jungle");
	XM1014Skin.AddItem("CaliCamo");
	XM1014Skin.AddItem("Pit Viper");
	XM1014Skin.AddItem("Tranquility");
	XM1014Skin.AddItem("Red Python");
	XM1014Skin.AddItem("Heaven Guard");
	XM1014Skin.AddItem("Red Leather");
	XM1014Skin.AddItem("Bone Machine");
	XM1014Skin.AddItem("Quicksilver");
	XM1014Skin.AddItem("Scumbria");
	XM1014Skin.AddItem("Teclu Burner");
	XM1014Skin.AddItem("Black Tie");
	Shotgungroup.PlaceLabledControl("XM1014", this, &XM1014Skin);

	SAWEDOFFSkin.SetFileId("sawedoff_skin");
	SAWEDOFFSkin.AddItem("First Class");
	SAWEDOFFSkin.AddItem("Forest DDPAT");
	SAWEDOFFSkin.AddItem("Contrast Spray");
	SAWEDOFFSkin.AddItem("Snake Camo");
	SAWEDOFFSkin.AddItem("Orange DDPAT");
	SAWEDOFFSkin.AddItem("Fade");
	SAWEDOFFSkin.AddItem("Copper");
	SAWEDOFFSkin.AddItem("Origami");
	SAWEDOFFSkin.AddItem("Sage Spray");
	SAWEDOFFSkin.AddItem("VariCamo");
	SAWEDOFFSkin.AddItem("Irradiated Alert");
	SAWEDOFFSkin.AddItem("Mosaico");
	SAWEDOFFSkin.AddItem("Serenity");
	SAWEDOFFSkin.AddItem("Amber Fade");
	SAWEDOFFSkin.AddItem("Full Stop");
	SAWEDOFFSkin.AddItem("Highwayman");
	SAWEDOFFSkin.AddItem("The Kraken");
	SAWEDOFFSkin.AddItem("Rust Coat");
	SAWEDOFFSkin.AddItem("Bamboo Shadow");
	SAWEDOFFSkin.AddItem("Bamboo Forest");
	SAWEDOFFSkin.AddItem("Yorick");
	SAWEDOFFSkin.AddItem("Fubar");
	SAWEDOFFSkin.AddItem("Wasteland Princess");
	Shotgungroup.PlaceLabledControl("Sawed-Off", this, &SAWEDOFFSkin);

	NOVASkin.SetFileId("nova_skin");
	NOVASkin.AddItem("Candy Apple");
	NOVASkin.AddItem("Blaze Orange");
	NOVASkin.AddItem("Modern Hunter");
	NOVASkin.AddItem("Forest Leaves");
	NOVASkin.AddItem("Bloomstick");
	NOVASkin.AddItem("Sand Dune");
	NOVASkin.AddItem("Polar Mesh");
	NOVASkin.AddItem("Walnut");
	NOVASkin.AddItem("Predator");
	NOVASkin.AddItem("Tempest");
	NOVASkin.AddItem("Graphite");
	NOVASkin.AddItem("Ghost Camo");
	NOVASkin.AddItem("Rising Skull");
	NOVASkin.AddItem("Antique");
	NOVASkin.AddItem("Green Apple");
	NOVASkin.AddItem("Caged Steel");
	NOVASkin.AddItem("Koi");
	NOVASkin.AddItem("Moon in Libra");
	NOVASkin.AddItem("Ranger");
	NOVASkin.AddItem("HyperBeast");
	Shotgungroup.PlaceLabledControl("Nova", this, &NOVASkin);
#pragma endregion

#pragma region Rifles
	Riflegroup.SetPosition(10, 280);
	Riflegroup.SetText("Rifles");
	Riflegroup.SetSize(250, 195);
	RegisterControl(&Riflegroup);

	AK47Skin.SetFileId("ak47_skin");
	AK47Skin.AddItem("First Class");
	AK47Skin.AddItem("Red Laminate");
	AK47Skin.AddItem("Case Hardened");
	AK47Skin.AddItem("Black Laminate");
	AK47Skin.AddItem("Fire Serpent");
	AK47Skin.AddItem("Cartel");
	AK47Skin.AddItem("Emerald Pinstripe");
	AK47Skin.AddItem("Blue Laminate");
	AK47Skin.AddItem("Redline");
	AK47Skin.AddItem("Vulcan");
	AK47Skin.AddItem("Jaguar");
	AK47Skin.AddItem("Jet Set");
	AK47Skin.AddItem("Wasteland Rebel");
	AK47Skin.AddItem("Elite Build");
	AK47Skin.AddItem("Hydroponic");
	AK47Skin.AddItem("Aquamarine Revenge");
	AK47Skin.AddItem("Frontside Misty");
	AK47Skin.AddItem("Point Disarray");
	AK47Skin.AddItem("Fuel Injector");
	AK47Skin.AddItem("Neon Revolution");
	Riflegroup.PlaceLabledControl("AK-47", this, &AK47Skin);

	M41SSkin.SetFileId("m4a1s_skin");
	M41SSkin.AddItem("Dark Water");
	M41SSkin.AddItem("Hyper Beast");
	M41SSkin.AddItem("Boreal Forest");
	M41SSkin.AddItem("VariCamo");
	M41SSkin.AddItem("Nitro");
	M41SSkin.AddItem("Bright Water");
	M41SSkin.AddItem("Atomic Alloy");
	M41SSkin.AddItem("Blood Tiger");
	M41SSkin.AddItem("Guardian");
	M41SSkin.AddItem("Master Piece");
	M41SSkin.AddItem("Knight");
	M41SSkin.AddItem("Cyrex");
	M41SSkin.AddItem("Basilisk");
	M41SSkin.AddItem("Icarus Fell");
	M41SSkin.AddItem("Hot Rod");
	M41SSkin.AddItem("Golden Coi");
	M41SSkin.AddItem("Chantico's Fire");
	M41SSkin.AddItem("Mecha Industries");
	M41SSkin.AddItem("Flashback");
	Riflegroup.PlaceLabledControl("M4A1-S", this, &M41SSkin);

	M4A4Skin.SetFileId("m4a4_skin");
	M4A4Skin.AddItem("Bullet Rain");
	M4A4Skin.AddItem("Zirka");
	M4A4Skin.AddItem("Asiimov");
	M4A4Skin.AddItem("Howl");
	M4A4Skin.AddItem("X-Ray");
	M4A4Skin.AddItem("Desert-Strike");
	M4A4Skin.AddItem("Griffin");
	M4A4Skin.AddItem("Dragon King");
	M4A4Skin.AddItem("Poseidon");
	M4A4Skin.AddItem("Daybreak");
	M4A4Skin.AddItem("Evil Daimyo");
	M4A4Skin.AddItem("Royal Paladin");
	M4A4Skin.AddItem("The BattleStar");
	M4A4Skin.AddItem("Desolate Space");
	M4A4Skin.AddItem("Buzz Kill");
	Riflegroup.PlaceLabledControl("M4A4", this, &M4A4Skin);

	AUGSkin.SetFileId("aug_skin");
	AUGSkin.AddItem("Bengal Tiger");
	AUGSkin.AddItem("Hot Rod");
	AUGSkin.AddItem("Chameleon");
	AUGSkin.AddItem("Torque");
	AUGSkin.AddItem("Radiation Hazard");
	AUGSkin.AddItem("Asterion");
	AUGSkin.AddItem("Daedalus");
	AUGSkin.AddItem("Akihabara Accept");
	AUGSkin.AddItem("Ricochet");
	AUGSkin.AddItem("Fleet Flock");
	AUGSkin.AddItem("Syd Mead");
	Riflegroup.PlaceLabledControl("AUG", this, &AUGSkin);

	FAMASSkin.SetFileId("famas_skin");
	FAMASSkin.AddItem("Contrast Spray");
	FAMASSkin.AddItem("Colony");
	FAMASSkin.AddItem("Cyanospatter");
	FAMASSkin.AddItem("Djinn");
	FAMASSkin.AddItem("Afterimage");
	FAMASSkin.AddItem("Doomkitty");
	FAMASSkin.AddItem("Spitfire");
	FAMASSkin.AddItem("Teardown");
	FAMASSkin.AddItem("Hexane");
	FAMASSkin.AddItem("Pulse");
	FAMASSkin.AddItem("Sergeant");
	FAMASSkin.AddItem("Styx");
	FAMASSkin.AddItem("Neural Net");
	FAMASSkin.AddItem("Survivor");
	FAMASSkin.AddItem("Valence");
	FAMASSkin.AddItem("Roll Cage");
	FAMASSkin.AddItem("Mecha Industries");
	Riflegroup.PlaceLabledControl("FAMAS", this, &FAMASSkin);

	GALILSkin.SetFileId("galil_skin");
	GALILSkin.AddItem("Forest DDPAT");
	GALILSkin.AddItem("Contrast Spray");
	GALILSkin.AddItem("Orange DDPAT");
	GALILSkin.AddItem("Eco");
	GALILSkin.AddItem("Winter Forest");
	GALILSkin.AddItem("Sage Spray");
	GALILSkin.AddItem("VariCamo");
	GALILSkin.AddItem("VariCamo");
	GALILSkin.AddItem("Chatterbox");
	GALILSkin.AddItem("Shattered");
	GALILSkin.AddItem("Kami");
	GALILSkin.AddItem("Blue Titanium");
	GALILSkin.AddItem("Urban Rubble");
	GALILSkin.AddItem("Hunting Blind");
	GALILSkin.AddItem("Sandstorm");
	GALILSkin.AddItem("Tuxedo");
	GALILSkin.AddItem("Cerberus");
	GALILSkin.AddItem("Aqua Terrace");
	GALILSkin.AddItem("Rocket Pop");
	GALILSkin.AddItem("Stone Cold");
	GALILSkin.AddItem("Firefight");
	Riflegroup.PlaceLabledControl("GALIL", this, &GALILSkin);

	SG553Skin.SetFileId("sg552_skin");
	SG553Skin.AddItem("Bulldozer");
	SG553Skin.AddItem("Ultraviolet");
	SG553Skin.AddItem("Damascus Steel");
	SG553Skin.AddItem("Fallout Warning");
	SG553Skin.AddItem("Damascus Steel");
	SG553Skin.AddItem("Pulse");
	SG553Skin.AddItem("Army Sheen");
	SG553Skin.AddItem("Traveler");
	SG553Skin.AddItem("Fallout Warning");
	SG553Skin.AddItem("Cyrex");
	SG553Skin.AddItem("Tiger Moth");
	SG553Skin.AddItem("Atlas");
	Riflegroup.PlaceLabledControl("SG552", this, &SG553Skin);
#pragma endregion

#pragma region MPs
	MPGroup.SetPosition(520, 55);
	MPGroup.SetText("MPs");
	MPGroup.SetSize(250, 200);
	RegisterControl(&MPGroup);

	MAC10Skin.SetFileId("mac10_skin");
	MAC10Skin.AddItem("Tornado");
	MAC10Skin.AddItem("Candy Apple");
	MAC10Skin.AddItem("Silver");
	MAC10Skin.AddItem("Forest DDPAT");
	MAC10Skin.AddItem("Urban DDPAT");
	MAC10Skin.AddItem("Fade");
	MAC10Skin.AddItem("Neon Rider");
	MAC10Skin.AddItem("Ultraviolet");
	MAC10Skin.AddItem("Palm");
	MAC10Skin.AddItem("Graven");
	MAC10Skin.AddItem("Tatter");
	MAC10Skin.AddItem("Amber Fade");
	MAC10Skin.AddItem("Heat");
	MAC10Skin.AddItem("Curse");
	MAC10Skin.AddItem("Indigo");
	MAC10Skin.AddItem("Commuter");
	MAC10Skin.AddItem("Nuclear Garden");
	MAC10Skin.AddItem("Malachite");
	MAC10Skin.AddItem("Rangeen");
	MAC10Skin.AddItem("Lapis Gator");
	MPGroup.PlaceLabledControl("MAC-10", this, &MAC10Skin);

	P90Skin.SetFileId("p90_skin");
	P90Skin.AddItem("Leather");
	P90Skin.AddItem("Virus");
	P90Skin.AddItem("Contrast Spray");
	P90Skin.AddItem("Storm");
	P90Skin.AddItem("Cold Blooded");
	P90Skin.AddItem("Glacier Mesh");
	P90Skin.AddItem("Sand Spray");
	P90Skin.AddItem("Death by Kitty");
	P90Skin.AddItem("Ash Wood");
	P90Skin.AddItem("Fallout Warning");
	P90Skin.AddItem("Scorched");
	P90Skin.AddItem("Emerald Dragon");
	P90Skin.AddItem("Teardown");
	P90Skin.AddItem("Blind Spot");
	P90Skin.AddItem("Trigon");
	P90Skin.AddItem("Desert Warfare");
	P90Skin.AddItem("Module");
	P90Skin.AddItem("Asiimov");
	P90Skin.AddItem("Elite Build");
	P90Skin.AddItem("Shapewood");
	P90Skin.AddItem("Shallow Grave");
	MPGroup.PlaceLabledControl("P90", this, &P90Skin);

	UMP45Skin.SetFileId("ump45_skin");
	UMP45Skin.AddItem("Blaze");
	UMP45Skin.AddItem("Forest DDPAT");
	UMP45Skin.AddItem("Gunsmoke");
	UMP45Skin.AddItem("Urban DDPAT");
	UMP45Skin.AddItem("Grand Prix");
	UMP45Skin.AddItem("Carbon Fiber");
	UMP45Skin.AddItem("Caramel");
	UMP45Skin.AddItem("Fallout Warning");
	UMP45Skin.AddItem("Scorched");
	UMP45Skin.AddItem("Bone Pile");
	UMP45Skin.AddItem("Delusion");
	UMP45Skin.AddItem("Corporal");
	UMP45Skin.AddItem("Indigo");
	UMP45Skin.AddItem("Labyrinth");
	UMP45Skin.AddItem("Minotaur's Labyrinth");
	UMP45Skin.AddItem("Riot");
	UMP45Skin.AddItem("Primal Saber");
	MPGroup.PlaceLabledControl("UMP-45", this, &UMP45Skin);

	BIZONSkin.SetFileId("bizon_skin");
	BIZONSkin.AddItem("Blue Streak");
	BIZONSkin.AddItem("Modern Hunter");
	BIZONSkin.AddItem("Forest Leaves");
	BIZONSkin.AddItem("Bone Mask");
	BIZONSkin.AddItem("Carbon Fiber");
	BIZONSkin.AddItem("Sand Dashed");
	BIZONSkin.AddItem("Urban Dashed");
	BIZONSkin.AddItem("Brass");
	BIZONSkin.AddItem("VariCamo");
	BIZONSkin.AddItem("Irradiated Alert");
	BIZONSkin.AddItem("Rust Coat");
	BIZONSkin.AddItem("Water Sigil");
	BIZONSkin.AddItem("Night Ops");
	BIZONSkin.AddItem("Cobalt Halftone");
	BIZONSkin.AddItem("Antique");
	BIZONSkin.AddItem("Rust Coat");
	BIZONSkin.AddItem("Osiris");
	BIZONSkin.AddItem("Chemical Green");
	BIZONSkin.AddItem("Bamboo Print");
	BIZONSkin.AddItem("Bamboo Forest");
	BIZONSkin.AddItem("Fuel Rod");
	BIZONSkin.AddItem("Photic Zone");
	BIZONSkin.AddItem("Judgement of Anubis");
	MPGroup.PlaceLabledControl("PP-Bizon", this, &BIZONSkin);

	MP7Skin.SetFileId("mp7_skin");
	MP7Skin.AddItem("Groundwater");
	MP7Skin.AddItem("Whiteout");
	MP7Skin.AddItem("Forest DDPAT");
	MP7Skin.AddItem("Anodized Navy");
	MP7Skin.AddItem("Skulls");
	MP7Skin.AddItem("Gunsmoke");
	MP7Skin.AddItem("Contrast Spray");
	MP7Skin.AddItem("Bone Mask");
	MP7Skin.AddItem("Ossified");
	MP7Skin.AddItem("Orange Peel");
	MP7Skin.AddItem("VariCamo");
	MP7Skin.AddItem("Army Recon");
	MP7Skin.AddItem("Groundwater");
	MP7Skin.AddItem("Ocean Foam");
	MP7Skin.AddItem("Full Stop");
	MP7Skin.AddItem("Urban Hazard");
	MP7Skin.AddItem("Olive Plaid");
	MP7Skin.AddItem("Armor Core");
	MP7Skin.AddItem("Asterion");
	MP7Skin.AddItem("Nemesis");
	MP7Skin.AddItem("Special Delivery");
	MP7Skin.AddItem("Impire");
	MPGroup.PlaceLabledControl("MP7", this, &MP7Skin);

	MP9Skin.SetFileId("mp9_skin");
	MP9Skin.AddItem("Ruby Poison Dart");
	MP9Skin.AddItem("Bone Mask");
	MP9Skin.AddItem("Hot Rod");
	MP9Skin.AddItem("Storm");
	MP9Skin.AddItem("Bulldozer");
	MP9Skin.AddItem("Hypnotic");
	MP9Skin.AddItem("Sand Dashed");
	MP9Skin.AddItem("Orange Peel");
	MP9Skin.AddItem("Dry Season");
	MP9Skin.AddItem("Dark Age");
	MP9Skin.AddItem("Rose Iron");
	MP9Skin.AddItem("Green Plaid");
	MP9Skin.AddItem("Setting Sun");
	MP9Skin.AddItem("Dart");
	MP9Skin.AddItem("Deadly Poison");
	MP9Skin.AddItem("Pandora's Box");
	MP9Skin.AddItem("Bioleak");
	MP9Skin.AddItem("Airlock");
	MPGroup.PlaceLabledControl("MP9", this, &MP9Skin);

#pragma endregion

#pragma region Pistols
	PistolGroup.SetPosition(770, 55);
	PistolGroup.SetText("Pistols");
	PistolGroup.SetSize(250, 235);
	RegisterControl(&PistolGroup);

	// Glock-18
	GLOCKSkin.SetFileId("glock_skin");
	GLOCKSkin.AddItem("Groundwater");
	GLOCKSkin.AddItem("Candy Apple");
	GLOCKSkin.AddItem("Fade");
	GLOCKSkin.AddItem("Night");
	GLOCKSkin.AddItem("Dragon Tattoo");
	GLOCKSkin.AddItem("Twilight Galaxy");
	GLOCKSkin.AddItem("Sand Dune");
	GLOCKSkin.AddItem("Brass");
	GLOCKSkin.AddItem("Catacombs");
	GLOCKSkin.AddItem("Sand Dune");
	GLOCKSkin.AddItem("Steel Disruption");
	GLOCKSkin.AddItem("Blue Fissure");
	GLOCKSkin.AddItem("Death Rattle");
	GLOCKSkin.AddItem("Water Elemental");
	GLOCKSkin.AddItem("Reactor");
	GLOCKSkin.AddItem("Grinder");
	GLOCKSkin.AddItem("Bunsen Burner");
	GLOCKSkin.AddItem("Wraith");
	GLOCKSkin.AddItem("Royal Legion");
	GLOCKSkin.AddItem("Weasel");
	GLOCKSkin.AddItem("Wasteland Rebel");
	PistolGroup.PlaceLabledControl("Glock", this, &GLOCKSkin);

	// USP-S
	USPSSkin.SetFileId("usps_skin");
	USPSSkin.AddItem("Forest Leaves");
	USPSSkin.AddItem("Dark Water");
	USPSSkin.AddItem("VariCamo");
	USPSSkin.AddItem("Overgrowth");
	USPSSkin.AddItem("Caiman");
	USPSSkin.AddItem("Blood Tiger");
	USPSSkin.AddItem("Serum");
	USPSSkin.AddItem("Night Ops");
	USPSSkin.AddItem("Stainless");
	USPSSkin.AddItem("Guardian");
	USPSSkin.AddItem("Orion");
	USPSSkin.AddItem("Road Rash");
	USPSSkin.AddItem("Royal Blue");
	USPSSkin.AddItem("Business Class");
	USPSSkin.AddItem("Para Green");
	USPSSkin.AddItem("Torque");
	USPSSkin.AddItem("Kill Confirmed");
	USPSSkin.AddItem("Lead Conduit");
	USPSSkin.AddItem("Cyrex");
	PistolGroup.PlaceLabledControl("USP-S", this, &USPSSkin);

	// Deagle
	DEAGLESkin.SetFileId("deagle_skin");
	DEAGLESkin.AddItem("Blaze");
	DEAGLESkin.AddItem("Pilot");
	DEAGLESkin.AddItem("Midnight Storm");
	DEAGLESkin.AddItem("Sunset Storm");
	DEAGLESkin.AddItem("Forest DDPAT");
	DEAGLESkin.AddItem("Crimson Web");
	DEAGLESkin.AddItem("Urban DDPAT");
	DEAGLESkin.AddItem("Night");
	DEAGLESkin.AddItem("Hypnotic");
	DEAGLESkin.AddItem("Mudder");
	DEAGLESkin.AddItem("VariCamo");
	DEAGLESkin.AddItem("Golden Koi");
	DEAGLESkin.AddItem("Cobalt Disruption");
	DEAGLESkin.AddItem("Urban Rubble");
	DEAGLESkin.AddItem("Naga");
	DEAGLESkin.AddItem("Hand Cannon");
	DEAGLESkin.AddItem("Heirloom");
	DEAGLESkin.AddItem("Meteorite");
	DEAGLESkin.AddItem("Conspiracy");
	DEAGLESkin.AddItem("Bronze Deco");
	DEAGLESkin.AddItem("Sunset Storm");
	DEAGLESkin.AddItem("Corinthian");
	DEAGLESkin.AddItem("Kumicho Dragon");
	PistolGroup.PlaceLabledControl("Deagle", this, &DEAGLESkin);
#pragma region GloveChanger

	GloveGroup.SetPosition(700, 290);
	GloveGroup.SetText("Gloves");
	GloveGroup.SetSize(200, 215);
	RegisterControl(&GloveGroup);

	GloveActive.SetFileId("Glove_Active");
	GloveGroup.PlaceLabledControl("Glove Active", this, &GloveActive);

	GloveModel.SetFileId("glove_model");
	GloveModel.AddItem("Bloodhound");
	GloveModel.AddItem("Sporty");
	GloveModel.AddItem("Slick");
	GloveModel.AddItem("Specialist");
	GloveModel.AddItem("Moto");
	GloveModel.AddItem("Handwrap");
	GloveGroup.PlaceLabledControl("Glove Model", this, &GloveModel);

	GloveSkin.SetFileId("glove_skin");
	GloveSkin.AddItem("Bloodhound Black"); //0
	GloveSkin.AddItem("Bloodhound Snakeskin"); //1
	GloveSkin.AddItem("Bloodhound Metallic"); //2
	GloveSkin.AddItem("Sporty Purple"); //3
	GloveSkin.AddItem("Sporty Green"); //4
	GloveSkin.AddItem("Sporty Light Blue"); //5
	GloveSkin.AddItem("Sporty Military"); //6
	GloveSkin.AddItem("Slick Black"); //7
	GloveSkin.AddItem("Slick Military"); //8
	GloveSkin.AddItem("Slick Red"); //9
	GloveSkin.AddItem("Specialist DDPAT"); //10
	GloveSkin.AddItem("Specialist Kimono"); //11
	GloveSkin.AddItem("Specialist Emerald"); //12
	GloveSkin.AddItem("Specialist Orange"); //13
	GloveSkin.AddItem("Moto Mint"); //14
	GloveSkin.AddItem("Moto Boom"); //15
	GloveSkin.AddItem("Moto Blue"); //16
	GloveSkin.AddItem("Moto Black"); //17
	GloveSkin.AddItem("Moto Black"); //17
	GloveSkin.AddItem("Handwrap Grey Camo"); //18

	GloveSkin.AddItem("Handwrap Slaughter"); //19
	GloveSkin.AddItem("Handwrap Orange Camo"); //20
	GloveSkin.AddItem("Handwrap Leathery"); //21



	GloveGroup.PlaceLabledControl("Glove Skin", this, &GloveSkin);

	GloveApply.SetText("Apply");
	GloveApply.SetCallback(KnifeApplyCallbk);
	GloveApply.SetPosition(518, 450);
	GloveApply.SetSize(360, 106);
	RegisterControl(&GloveApply);

#pragma endregion Gloves Shit 
	// Dual Berettas
	DUALSSkin.SetFileId("duals_skin");
	DUALSSkin.AddItem("Anodized Navy");
	DUALSSkin.AddItem("Ossified");
	DUALSSkin.AddItem("Stained");
	DUALSSkin.AddItem("Contractor");
	DUALSSkin.AddItem("Colony");
	DUALSSkin.AddItem("Demolition");
	DUALSSkin.AddItem("Dualing Dragons");
	DUALSSkin.AddItem("Black Limba");
	DUALSSkin.AddItem("Red Quartz");
	DUALSSkin.AddItem("Cobalt Quartz");
	DUALSSkin.AddItem("Hemoglobin");
	DUALSSkin.AddItem("Urban Shock");
	DUALSSkin.AddItem("Marina");
	DUALSSkin.AddItem("Panther");
	DUALSSkin.AddItem("Retribution");
	DUALSSkin.AddItem("Briar");
	DUALSSkin.AddItem("Duelist");
	DUALSSkin.AddItem("Moon in Libra");
	DUALSSkin.AddItem("Cartel");
	DUALSSkin.AddItem("Ventilators");
	DUALSSkin.AddItem("Royal Consorts");
	DUALSSkin.AddItem("Cobra Strike");
	PistolGroup.PlaceLabledControl("Duals", this, &DUALSSkin);

	// Five Seven
	FIVESEVENSkin.SetFileId("fiveseven_skin");
	FIVESEVENSkin.AddItem("Candy Apple");
	FIVESEVENSkin.AddItem("Bone Mask");
	FIVESEVENSkin.AddItem("Case Hardened");
	FIVESEVENSkin.AddItem("Contractor");
	FIVESEVENSkin.AddItem("Forest Night");
	FIVESEVENSkin.AddItem("Orange Peel");
	FIVESEVENSkin.AddItem("Jungle");
	FIVESEVENSkin.AddItem("Nitro");
	FIVESEVENSkin.AddItem("Red Quartz");
	FIVESEVENSkin.AddItem("Anodized Gunmetal");
	FIVESEVENSkin.AddItem("Nightshade");
	FIVESEVENSkin.AddItem("Silver Quartz");
	FIVESEVENSkin.AddItem("Kami");
	FIVESEVENSkin.AddItem("Copper Galaxy");
	FIVESEVENSkin.AddItem("Neon Kimono");
	FIVESEVENSkin.AddItem("Fowl Play");
	FIVESEVENSkin.AddItem("Hot Shot");
	FIVESEVENSkin.AddItem("Urban Hazard");
	FIVESEVENSkin.AddItem("Monkey Business");
	FIVESEVENSkin.AddItem("Retrobution");
	FIVESEVENSkin.AddItem("Triumvirate");
	FIVESEVENSkin.AddItem("Capillary");
	FIVESEVENSkin.AddItem("Hyper Beast");
	PistolGroup.PlaceLabledControl("Five-Seven", this, &FIVESEVENSkin);

	TECNINESkin.SetFileId("tec9_skin");
	TECNINESkin.AddItem("Tornado");
	TECNINESkin.AddItem("Groundwater");
	TECNINESkin.AddItem("Forest DDPAT");
	TECNINESkin.AddItem("Terrace");
	TECNINESkin.AddItem("Urban DDPAT");
	TECNINESkin.AddItem("Ossified");
	TECNINESkin.AddItem("Hades");
	TECNINESkin.AddItem("Brass");
	TECNINESkin.AddItem("VariCamo");
	TECNINESkin.AddItem("Nuclear Threat");
	TECNINESkin.AddItem("Red Quartz");
	TECNINESkin.AddItem("Tornado");
	TECNINESkin.AddItem("Blue Titanium");
	TECNINESkin.AddItem("Army Mesh");
	TECNINESkin.AddItem("Titanium Bit");
	TECNINESkin.AddItem("Sandstorm");
	TECNINESkin.AddItem("Isaac");
	TECNINESkin.AddItem("Toxic");
	TECNINESkin.AddItem("Bamboo Forest");
	TECNINESkin.AddItem("Avalanche");
	TECNINESkin.AddItem("Jambiya");
	TECNINESkin.AddItem("Re-Entry");
	TECNINESkin.AddItem("Fuel Injector");
	PistolGroup.PlaceLabledControl("Tec-9", this, &TECNINESkin);

	P2000Skin.SetFileId("p2000_skin");
	P2000Skin.AddItem("Grassland Leaves");
	P2000Skin.AddItem("Silver");
	P2000Skin.AddItem("Granite Marbleized");
	P2000Skin.AddItem("Forest Leaves");
	P2000Skin.AddItem("Ossified");
	P2000Skin.AddItem("Handgun");
	P2000Skin.AddItem("Fade");
	P2000Skin.AddItem("Scorpion");
	P2000Skin.AddItem("Grassland");
	P2000Skin.AddItem("Corticera");
	P2000Skin.AddItem("Ocean Foam");
	P2000Skin.AddItem("Pulse");
	P2000Skin.AddItem("Amber Fade");
	P2000Skin.AddItem("Red FragCam");
	P2000Skin.AddItem("Chainmail");
	P2000Skin.AddItem("Coach Class");
	P2000Skin.AddItem("Ivory");
	P2000Skin.AddItem("Fire Elemental");
	P2000Skin.AddItem("Asterion");
	P2000Skin.AddItem("Pathfinder");
	P2000Skin.AddItem("Imperial");
	P2000Skin.AddItem("Oceanic");
	P2000Skin.AddItem("Imperial Dragon");
	PistolGroup.PlaceLabledControl("P2000", this, &P2000Skin);

	P250Skin.SetFileId("p250_skin");
	P250Skin.AddItem("Whiteout");
	P250Skin.AddItem("Metallic DDPAT");
	P250Skin.AddItem("Splash");
	P250Skin.AddItem("Gunsmoke");
	P250Skin.AddItem("Modern Hunter");
	P250Skin.AddItem("Bone Mask");
	P250Skin.AddItem("Boreal Forest");
	P250Skin.AddItem("Sand Dune");
	P250Skin.AddItem("Nuclear Threat");
	P250Skin.AddItem("Mehndi");
	P250Skin.AddItem("Facets");
	P250Skin.AddItem("Hive");
	P250Skin.AddItem("Muertos");
	P250Skin.AddItem("Steel Disruption");
	P250Skin.AddItem("Undertow");
	P250Skin.AddItem("Franklin");
	P250Skin.AddItem("Neon Kimono");
	P250Skin.AddItem("Supernova");
	P250Skin.AddItem("Contamination");
	P250Skin.AddItem("Cartel");
	P250Skin.AddItem("Valence");
	P250Skin.AddItem("Crimson Kimono");
	P250Skin.AddItem("Mint Kimono");
	P250Skin.AddItem("Wing Shot");
	P250Skin.AddItem("Asiimov");
	PistolGroup.PlaceLabledControl("P250", this, &P250Skin);

#pragma endregion
/*
#pragma region Skinsettings
	SkinsettingsGroup.SetPosition(760, 290);
	SkinsettingsGroup.SetText("Settings");
	SkinsettingsGroup.SetSize(295, 115);
	RegisterControl(&SkinsettingsGroup);

	StatTrakEnable.SetFileId("skin_stattrack");
	SkinsettingsGroup.PlaceLabledControl("Stat Track", this, &StatTrakEnable);

	StatTrackAmount.SetFileId("skin_stamount");
	SkinsettingsGroup.PlaceLabledControl("Value", this, &StatTrackAmount);

	KnifeName.SetFileId("knife_name");
	SkinsettingsGroup.PlaceLabledControl("Knife Name", this, &KnifeName);

	SkinName.SetFileId("skin_name");
	SkinsettingsGroup.PlaceLabledControl("Skin Name", this, &SkinName);

#pragma endregion
*/
}

void CMiscTab::Setup()
{
	SetTitle("G");

#pragma region Other
	OtherGroup.SetPosition(270, 55);
	OtherGroup.SetSize(270, 350);
	OtherGroup.SetText("Misc");
	RegisterControl(&OtherGroup);
	EzTextMisc.SetPosition(500, 30);
	EzTextMisc.SetText("SUICIDE.CC");
	//EzText.SetSize(10, 10);
	RegisterControl(&EzTextMisc);
	OtherChatSpam.SetFileId("otr_spam");
	OtherChatSpam.AddItem("Off");
	OtherChatSpam.AddItem("Normal");
	
	OtherGroup.PlaceLabledControl("Chat Spam", this, &OtherChatSpam);









	OtherNameSpam.SetFileId("otr_spam");
	OtherNameSpam.AddItem("Off");
	OtherNameSpam.AddItem("SUICIDE.CC");
	OtherNameSpam.AddItem("SUICIDE.CC Colored");
	OtherNameSpam.AddItem("Blank");
	OtherNameSpam.AddItem("PU Sucks");
	OtherNameSpam.AddItem("AIMWARE");
	OtherNameSpam.AddItem("InterWebz");
	OtherGroup.PlaceLabledControl("Name Spam", this, &OtherNameSpam);







	OtherClantag.SetFileId("otr_spam");
	OtherClantag.AddItem("Off");
	OtherClantag.AddItem("SUICIDE.CC");
	OtherClantag.AddItem("Blank");
	OtherClantag.AddItem("Valve");
	OtherClantag.AddItem("SUICIDE.CC Seach mode");
	OtherClantag.AddItem("Time WH");
	OtherGroup.PlaceLabledControl("Custom Clantag", this, &OtherClantag);

	OtherClantagspeed.SetFileId("otr_clantagspeed");
	OtherClantagspeed.SetBoundaries(0, 3);
	OtherClantagspeed.SetValue(1);
	OtherGroup.PlaceLabledControl("Clantag Speed", this, &OtherClantagspeed);

	OtherTeamChat.SetFileId("otr_teamchat");
	OtherGroup.PlaceLabledControl("Team Chat Only", this, &OtherTeamChat);

	OtherChatDelay.SetFileId("otr_chatdelay");
	OtherChatDelay.SetBoundaries(0.1, 3.0);
	OtherChatDelay.SetValue(0.5);
	OtherGroup.PlaceLabledControl("Spam Delay", this, &OtherChatDelay);

	OtherAirStuck.SetFileId("otr_astuck");
	OtherGroup.PlaceLabledControl("Air Stuck", this, &OtherAirStuck);

	OtherSpectators.SetFileId("otr_speclist");
	OtherGroup.PlaceLabledControl("Spectators List", this, &OtherSpectators);

	OtherSafeMode.SetFileId("otr_safemode");
	OtherSafeMode.SetState(true);
	OtherGroup.PlaceLabledControl("Anti Untrusted", this, &OtherSafeMode);

	AutoAccept.SetFileId("otr_AutoAccept");
	OtherGroup.PlaceLabledControl("Auto Accept", this, &AutoAccept);
	MoonWalk.SetFileId("otr_moonwalk");
	OtherGroup.PlaceLabledControl("Moon Walk", this, &MoonWalk);
	

	OtherHitmarker.SetFileId("hitmarker"); // Antiaim List for Yaw
	OtherHitmarker.AddItem("Disabled");
	OtherHitmarker.AddItem("Regular");
	OtherHitmarker.AddItem("Anime");
	OtherHitmarker.AddItem("p$ilent");
	OtherHitmarker.AddItem("Roblox");
	OtherGroup.PlaceOtherControl("Hitmarker", this, &OtherHitmarker);

	OtherKillSpam.SetFileId("otr_roundsay");
	OtherGroup.PlaceOtherControl("Hit Sounds", this, &OtherKillSpam);

	OtherKillSpamchat.SetFileId("otr_roundsay");
	OtherGroup.PlaceOtherControl("Kill Chat Spam", this, &OtherKillSpamchat);

#pragma endregion other random options

#pragma region FakeLag
	FakeLagGroup.SetPosition(10, 55);
	FakeLagGroup.SetSize(250, 80);
	FakeLagGroup.SetText("Fake Lag");
	RegisterControl(&FakeLagGroup);

	FakeLagEnable.SetFileId("fakelag_enable");
	FakeLagGroup.PlaceLabledControl("Enable", this, &FakeLagEnable);

	FakeLagChoke.SetFileId("fakelag_choke");
	FakeLagChoke.SetBoundaries(0, 10);
	FakeLagChoke.SetValue(0);
	FakeLagGroup.PlaceLabledControl("Factor", this, &FakeLagChoke);
#pragma endregion fakelag shit

#pragma region Strafing
	StrafingGroup.SetPosition(10, 145);
	StrafingGroup.SetSize(250, 150);
	StrafingGroup.SetText("Other");
	RegisterControl(&StrafingGroup);

	OtherAutoJump.SetFileId("otr_autojump");
	OtherAutoJump.AddItem("Off");
	OtherAutoJump.AddItem("Normal");
	StrafingGroup.PlaceLabledControl("Auto Jump", this, &OtherAutoJump);

	OtherAutoStrafe.SetFileId("otr_strafe");
	OtherAutoStrafe.AddItem("Off");
	OtherAutoStrafe.AddItem("Legit");
	OtherAutoStrafe.AddItem("Rage");
	StrafingGroup.PlaceLabledControl("Auto Strafer", this, &OtherAutoStrafe);

	OtherStrafeSpeed.SetFileId("acc_strafespeed");
	OtherStrafeSpeed.SetBoundaries(0.f, 10.f);
	OtherStrafeSpeed.SetValue(5.f);
	StrafingGroup.PlaceLabledControl("Strafe Speed", this, &OtherStrafeSpeed);

	OtherEdgeJump.SetFileId("otr_edgejump");
	StrafingGroup.PlaceLabledControl("Edge Jump", this, &OtherEdgeJump);
#pragma endregion

#pragma region Rank
	RankGroup.SetPosition(10, 260);
	RankGroup.SetSize(250, 50);
	RankGroup.SetText("Rank");
	RegisterControl(&RankGroup);

	RankRevealer.SetFileId("otr_rankreveal");
	RankGroup.PlaceLabledControl("Reveal Rank", this, &RankRevealer);
#pragma endregion
}

void CGUITab::Setup()
{
	SetTitle("H");


	EzTextColors.SetPosition(500, 30);
	EzTextColors.SetText("SUICIDE.CC");
	RegisterControl(&EzTextColors);




	ColorMain.SetPosition(16, 55);
	ColorMain.SetSize(260, 100);
	ColorMain.SetText("Filled Box Color");
	RegisterControl(&ColorMain);

	FillBoxRed.SetBoundaries(0.f, 255.f);
	FillBoxRed.SetValue(0.f);
	ColorMain.PlaceLabledControl("Red", this, &FillBoxRed);
	FillBoxGreen.SetBoundaries(0.f, 255.f);
	FillBoxGreen.SetValue(0.f);
	ColorMain.PlaceLabledControl("Green", this, &FillBoxGreen);
	FillBoxBlue.SetBoundaries(0.f, 255.f);
	FillBoxBlue.SetValue(0.f);
	ColorMain.PlaceLabledControl("Blue", this, &FillBoxBlue);
	FillBoxAlpha.SetBoundaries(0.f, 255.f);
	FillBoxAlpha.SetValue(144.f);
	ColorMain.PlaceLabledControl("Alpha", this, &FillBoxAlpha);


	Radar.SetPosition(296, 55);
	Radar.SetSize(260, 250);
	Radar.SetText("Radar Colors");
	RegisterControl(&Radar);

	TColorVisR.SetBoundaries(0.f, 255.f);
	TColorVisR.SetValue(255.f);
	Radar.PlaceLabledControl("T Red Vis", this, &TColorVisR);
	TColorVisG.SetBoundaries(0.f, 255.f);
	TColorVisG.SetValue(0.f);
	Radar.PlaceLabledControl("T Green Vis", this, &TColorVisG);
	TColorVisB.SetBoundaries(0.f, 255.f);
	TColorVisB.SetValue(0.f);
	Radar.PlaceLabledControl("T Blue Vis", this, &TColorVisB);

	TColorNoVisR.SetBoundaries(0.f, 255.f);
	TColorNoVisR.SetValue(0.f);
	Radar.PlaceLabledControl("T Red NoVis", this, &TColorNoVisR);
	TColorNoVisG.SetBoundaries(0.f, 255.f);
	TColorNoVisG.SetValue(0.f);
	Radar.PlaceLabledControl("T Green NoVis", this, &TColorNoVisG);
	TColorNoVisB.SetBoundaries(0.f, 255.f);
	TColorNoVisB.SetValue(100.f);
	Radar.PlaceLabledControl("T Blue NoVis", this, &TColorNoVisB);


	CTColorVisR.SetBoundaries(0.f, 255.f);
	CTColorVisR.SetValue(0.f);
	Radar.PlaceLabledControl("CT Red Vis", this, &CTColorVisR);
	CTColorVisG.SetBoundaries(0.f, 255.f);
	CTColorVisG.SetValue(255.f);
	Radar.PlaceLabledControl("CT Green Vis", this, &CTColorVisG);
	CTColorVisB.SetBoundaries(0.f, 255.f);
	CTColorVisB.SetValue(0.f);
	Radar.PlaceLabledControl("CT Blue Vis", this, &CTColorVisB);

	CTColorNoVisR.SetBoundaries(0.f, 255.f);
	CTColorNoVisR.SetValue(0.f);
	Radar.PlaceLabledControl("CT Red NoVis", this, &CTColorNoVisR);
	CTColorNoVisG.SetBoundaries(0.f, 255.f);
	CTColorNoVisG.SetValue(0.f);
	Radar.PlaceLabledControl("CT Green NoVis", this, &CTColorNoVisG);
	CTColorNoVisB.SetBoundaries(0.f, 255.f);
	CTColorNoVisB.SetValue(255.f);
	Radar.PlaceLabledControl("CT Blue NoVis", this, &CTColorNoVisB);


	ColorBulletTrace.SetPosition(16, 165);
	ColorBulletTrace.SetSize(260, 80);
	ColorBulletTrace.SetText("Bullet Trace Color");
	RegisterControl(&ColorBulletTrace);

	RBT.SetBoundaries(0.f, 255.f);
	RBT.SetValue(0.f);
	ColorBulletTrace.PlaceLabledControl("Red", this, &RBT);
	GBT.SetBoundaries(0.f, 255.f);
	GBT.SetValue(0.f);
	ColorBulletTrace.PlaceLabledControl("Green", this, &GBT);
	BBT.SetBoundaries(0.f, 255.f);
	BBT.SetValue(0.f);
	ColorBulletTrace.PlaceLabledControl("Blue", this, &BBT);

	NoScope.SetPosition(16, 255);
	NoScope.SetSize(260, 80);
	NoScope.SetText("NoScope Color lines");
	RegisterControl(&ColorBulletTrace);

	NoScopeR.SetBoundaries(0.f, 255.f);
	NoScopeR.SetValue(0.f);
	NoScope.PlaceLabledControl("Red", this, &NoScopeR);
	NoScopeG.SetBoundaries(0.f, 255.f);
	NoScopeG.SetValue(0.f);
	NoScope.PlaceLabledControl("Green", this, &NoScopeG);
	NoScopeB.SetBoundaries(0.f, 255.f);
	NoScopeB.SetValue(0.f);
	NoScope.PlaceLabledControl("Blue", this, &NoScopeB);
	

};

void Menu::CheckVersionsCSGO() {
	unsigned int Build = 13596;
	static bool firsttime = true;

		if (firsttime)
		{
			//if(  Build == 13587 )	
			if (Build == 13596)
			{
				Menu::SetupMenu();
				firsttime = false;
				Interfaces::Engine->ClientCmd_Unrestricted("clear");
				Interfaces::Engine->ClientCmd_Unrestricted("echo Inject Successfully");
				Interfaces::Engine->ClientCmd_Unrestricted("echo Menu on  INSERT KEY");
				Interfaces::Engine->ClientCmd_Unrestricted("echo FakeWalk on SHIFT KEY");
				Interfaces::Engine->ClientCmd_Unrestricted("echo GoodLuck");



			}
			else
			{
				if (!Build == 13596) {
					MessageBox(NULL, "Please Update Hack Version", "SUICIDE.CC", MB_OK);
					abort();
				}
				Interfaces::Engine->ClientCmd_Unrestricted("clear");
				Interfaces::Engine->ClientCmd_Unrestricted("echo You need upload your version software");
				UnLoadCallbk();
			


			}
		}
}


void Menu::SetupMenu()
{



	Window.Setup();

	GUI.RegisterWindow(&Window);
	GUI.BindWindow(VK_INSERT, &Window);
}

void Menu::DoUIFrame()
{
	// General Processing

	// If the "all filter is selected tick all the others
	if (Window.VisualsTab.FiltersAll.GetState())
	{
		Window.VisualsTab.FiltersC4.SetState(true);
		Window.VisualsTab.FiltersChickens.SetState(true);
		Window.VisualsTab.FiltersPlayers.SetState(true);
		Window.VisualsTab.FiltersWeapons.SetState(true);
	}

	GUI.Update();
	GUI.Draw();

}