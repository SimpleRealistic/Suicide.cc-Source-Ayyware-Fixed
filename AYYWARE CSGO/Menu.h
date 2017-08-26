#pragma once

#include "GUI.h"
#include "Controls.h"

class CRageBotTab : public CTab
{
public:
	void Setup();

	// Master Switch
	CLabel ActiveLabel;
	CCheckBox Active;

	// Aimbot Settings
	CGroupBox AimbotGroup;
	CGroupBox1 EzText;

	CCheckBox AimbotEnable;
	CCheckBox AimbotAutoFire;
	CSlider  AimbotFov;

	// Slient Aim Selection
	CComboBox AimbotSlientSelection;
	CCheckBox LagCompensation;
	CSlider  LagCompensationTick;
	CCheckBox AimbotAutoPistol;
	CCheckBox AimbotAutoRevolver;
	CCheckBox AimbotAimStep;
	CCheckBox AimbotKeyPress;
	CKeyBind  AimbotKeyBind;
	CKeyBind  AimbotStopKey;
	CCheckBox AutoAwpBody;
	CComboBox PreferBodyAim;
	// Target Selection Settings
	CGroupBox TargetGroup;

	//HitScanFutures
	CGroupBox HitScan;
	CGroupBox Edges;
	CGroupBox HitScanMenu;
	CGroupBox ResolverMenu;


	CSlider Shot1;
	CSlider Shot2;
	CSlider Shot3;
	CSlider Shot4;
	CSlider Shot5;


	CComboBox TargetSelection;
	CCheckBox TargetFriendlyFire;
	CComboBox TargetHitbox;
	CComboBox TargetHitscan;
	CCheckBox TargetMultipoint;
	CSlider   TargetPointscale;



	CCheckBox PVSFix;

	//HitScan Hitboxes
	CCheckBox HitScanHead;
	CCheckBox HitScanNeck;
	CCheckBox HitScanChest;
	CCheckBox HitScanPelvis;
	CCheckBox HitScanStomach;
	CCheckBox HitScanArms;
	CCheckBox HitScanLegs;

	//Edges
	CCheckBox HeadEdge;
	CCheckBox NeckEdge;
	CCheckBox ChestEdge;
	CCheckBox PelvisEdge;
	CCheckBox StomachEdge;
	CCheckBox ArmsEdge;
	CCheckBox LegsEdge;


	// Accuracy Settings
	CGroupBox AccuracyGroup;
	CCheckBox AccuracyRecoil;
	CCheckBox AccuracySpread;
	CCheckBox AccuracyAutoWall;
	CSlider  AccuracyMinimumDamage;
	CCheckBox AccuracyAutoStop;
	CCheckBox AccuracyAutoCrouch;
	CCheckBox AccuracyAutoScope;
	CSlider  AccuracyHitchance;
	CComboBox AccuracyResolverPitch;
	CComboBox AccuracyResolverYaw;
	CComboBox AccuracyResolverAuto;
	CCheckBox FakeLagFix;
	CSlider AccuracySmart;

	// Engine Hake
	CCheckBox PositionAccuracy;
	CCheckBox PerfectAccuracy;
	CCheckBox AccuracyPrediction;
	CCheckBox AccuracyLagCompensate;

	// Anti-Aim Settings
	CGroupBox AntiAimGroup;
	CCheckBox AntiAimEnable;

	CComboBox AntiAimPitch;
	CComboBox AntiAimYaw;
	CComboBox AntiAimYawFake;
	CComboBox AntiAimReady;
	CCheckBox AntiAimJitterLBY;
	CCheckBox AntiAimFakeWalk;
	CCheckBox MinusLBYADD;
	CSlider  AddLBYYAWFLIP;
	CSlider  RealAntiAimYawSlider;

	// CCheckBox AntiAimEdge;

	// CSlider	  AntiAimOffset;
	CSlider  AntiAimSpinspeed;
	CCheckBox AntiAimKnife;
	CCheckBox AntiAimTarget;
	CCheckBox AntiAimEdge;

};

class CLegitBotTab : public CTab
{
public:
	void Setup();

	// Master Switch
	CLabel ActiveLabel;
	CCheckBox Active;
	CGroupBox1 EzTextLegit;
	// Aimbot Settings
	CGroupBox AimbotGroup;
	CCheckBox AimbotEnable;
	CCheckBox AimbotAutoFire;
	CCheckBox AimbotFriendlyFire;
	CCheckBox AimbotKeyPress;
	CKeyBind  AimbotKeyBind;
	CCheckBox AimbotAutoPistol;
	CSlider   AimbotInaccuracy;
	CKeyBind  AimbotDisableVis;

	// Main
	CGroupBox TriggerGroup;
	CCheckBox TriggerEnable;
	CCheckBox TriggerKeyPress;
	CKeyBind  TriggerKeyBind;
	CSlider   TriggerDelay;

	// Main
	CGroupBox WeaponMainGroup;
	CSlider   WeaponMainSpeed;
	CSlider   WeaponMainFoV;
	CCheckBox WeaponMainRecoil;
	CCheckBox WeaponMainPSilent;
	CSlider   WeaponMainInacc;
	CComboBox WeaponMainHitbox;

	// Pistol
	CGroupBox WeaponPistGroup;
	CSlider   WeaponPistSpeed;
	CSlider   WeaponPistFoV;
	CCheckBox WeaponPistRecoil;
	CCheckBox WeaponPistPSilent;
	CSlider   WeaponPistInacc;
	CComboBox WeaponPistHitbox;

	// Sniper
	CGroupBox WeaponSnipGroup;
	CSlider   WeaponSnipSpeed;
	CSlider   WeaponSnipFoV;
	CCheckBox WeaponSnipRecoil;
	CCheckBox WeaponSnipPSilent;
	CSlider   WeaponSnipInacc;
	CComboBox WeaponSnipHitbox;
};

class CVisualTab : public CTab
{
public:
	void Setup();
	CGroupBox1 EzTextVisual;
	CGroupBox StaffGroup;
	// Master Switch
	CComboBox OtherThirdpersonAngle;
	CLabel ActiveLabel;
	CCheckBox Active;
	CKeyBind OtherThirdpersonKeyBind;
	CKeyBind OtherThirdpersonKeyStop;
	CSlider OtherThirdpersonRange;
	CCheckBox IsScoped;
	CCheckBox HasDefuser;
	// Options Settings
	CGroupBox OptionsGroup;
	CCheckBox OptionsBox;
	CCheckBox Glow;
	CSlider GlowAlpha;
	CComboBox BoxDesign;
	CCheckBox OptionsLBY;

	CCheckBox OtherRadarEnemyOnly;
	CCheckBox OtherRadarVisibleOnly;
	CSlider OtherRadarScale;

	CCheckBox BulletTrace;
	CSlider BulletTraceLength;
	CCheckBox Grenades;
	CCheckBox IsDefusing;
	CCheckBox OptionsName;
	CCheckBox OptionHealthEnable;
	CComboBox OptionsHealth;
	CCheckBox OptionsWeapon;
	CCheckBox OptionsInfo;
	CCheckBox OptionsHelmet;
	CCheckBox OptionsKit;
	CCheckBox OptionsDefuse;
	//CCheckBox OptionsGlow;
	CComboBox OptionsChams;
	CCheckBox OptionsSkeleton;
	CCheckBox OptionsAimSpot;
	CCheckBox OptionsCompRank;

	// Skeleton ESP Colour
	CGroupBox ESPColorSkeleton;
	CComboBox ControlColor;
	CSlider   RSkele;
	CSlider   GSkele;
	CSlider   BSkele;
	CSlider   ASkele;

	// Box ESP Colour
	CGroupBox ESPColorBox;
	CSlider   RBoxCT;
	CSlider   GBoxCT;
	CSlider   BBoxCT;
	CSlider   ABoxCT;
	CSlider   RBoxCTV;
	CSlider   GBoxCTV;
	CSlider   BBoxCTV;
	CSlider   ABoxCTV;

	// Filters Settings
	CGroupBox FiltersGroup;
	CCheckBox FiltersAll;
	CCheckBox FiltersPlayers;
	CCheckBox FiltersEnemiesOnly;
	CCheckBox FiltersWeapons;
	CCheckBox FiltersChickens;
	CCheckBox FiltersC4;

	// Other Settings
	CGroupBox OtherGroup;
	CComboBox OtherCrosshair;
	CComboBox OtherRecoilCrosshair;
	CCheckBox OtherHitlerCrosshair;
	CCheckBox OtherHitmarker;
	CCheckBox OtherRadar;
	CCheckBox OtherNoVisualRecoil;
	CSlider OtherRadarXPosition;
	CSlider OtherRadarYPosition;
	CCheckBox Radar;
	CCheckBox OtherNoSky; 
	CCheckBox OptionsFillBox;
	CCheckBox OtherNoFlash; 
	CCheckBox OtherNoSmoke;
	CCheckBox OtherAsusWalls;
	CComboBox OtherNoHands;
	CCheckBox OtherThirdperson;
	CSlider  OtherViewmodelFOV;
	CSlider  OtherFOV;
	CComboBox OtherWorld;
	CCheckBox CustomSky;
	CCheckBox OtherNoScope;
	CCheckBox NightMode;

	CSlider   TraceLength;

};

class CMiscTab : public CTab
{
public:
	void Setup();
	
	// Other Settings
	CGroupBox OtherGroup;
	
	CCheckBox OtherSafeMode;
	CComboBox OtherChatSpam;
	CComboBox OtherNameSpam;
	CCheckBox OtherTeamChat;
	CSlider	  OtherChatDelay;
	CKeyBind  OtherAirStuck;
	CKeyBind  OtherLagSwitch;
	CCheckBox OtherSpectators;
	CGroupBox1 EzTextMisc;
	CCheckBox MoonWalk;
	CCheckBox AutoAccept;
	//CCheckBox OtherAutoAccept;
	CCheckBox OtherWalkbot;
	CComboBox OtherHitmarker;
	CCheckBox OtherKillSpam;
	CCheckBox OtherKillSpamchat;
	// Clan Tag Settings
	CComboBox OtherClantag;
	CSlider  OtherClantagspeed;

	// Fake Lag Settings
	CGroupBox FakeLagGroup;
	CCheckBox FakeLagEnable;
	CSlider FakeLagChoke;
	CSlider	  FakeLagSend;
	CCheckBox ChokeRandomize;
	CCheckBox SendRandomize;
	//CCheckBox FakeLagWhileShooting;

	// Teleport shit cause we're cool
	CGroupBox TeleportGroup;
	CCheckBox TeleportEnable;
	CKeyBind  TeleportKey;

	// Strafing
	CGroupBox StrafingGroup;
	CComboBox OtherAutoJump;
	CComboBox OtherAutoStrafe;
	CSlider   OtherStrafeSpeed;
	CCheckBox OtherEdgeJump;

	// Rank Reveal
	CGroupBox RankGroup;
	CCheckBox RankRevealer;
};

class CSkinTab : public CTab
{
public:
	void Setup();
	CGroupBox1 EzTextSkins;
	// Knife Changer/Skin Changer
	CLabel SkinActive;
	CCheckBox SkinEnable;
	CButton   SkinApply;

	// Knife
	CGroupBox KnifeGroup;
	CComboBox KnifeModel;
	CComboBox KnifeSkin;

	// Pistols
	CGroupBox PistolGroup;
	CComboBox GLOCKSkin;
	CComboBox USPSSkin;
	CComboBox DEAGLESkin;
	CComboBox MAGNUMSkin;
	CComboBox DUALSSkin;
	CComboBox FIVESEVENSkin;
	CComboBox TECNINESkin;
	CComboBox P2000Skin;
	CComboBox P250Skin;

	// MPs
	CGroupBox MPGroup;
	CComboBox MAC10Skin;
	CComboBox P90Skin;
	CComboBox UMP45Skin;
	CComboBox BIZONSkin;
	CComboBox MP7Skin;
	CComboBox MP9Skin;

	// Rifles
	CGroupBox Riflegroup;
	CComboBox M41SSkin;
	CComboBox M4A4Skin;
	CComboBox AK47Skin;
	CComboBox AUGSkin;
	CComboBox FAMASSkin;
	CComboBox GALILSkin;
	CComboBox SG553Skin;
	CButton   GloveApply;

	// Glove
	CGroupBox GloveGroup;
	CComboBox GloveModel;
	CComboBox GloveSkin;
	CCheckBox GloveActive;

	// Machineguns
	CGroupBox MachinegunsGroup;
	CComboBox NEGEVSkin;
	CComboBox M249Skin;

	// Snipers
	CGroupBox Snipergroup;
	CComboBox SCAR20Skin;
	CComboBox G3SG1Skin;
	CComboBox SSG08Skin;
	CComboBox AWPSkin;

	// Shotguns
	CGroupBox Shotgungroup;
	CComboBox MAG7Skin;
	CComboBox XM1014Skin;
	CComboBox SAWEDOFFSkin;
	CComboBox NOVASkin;
};

class CGUITab : public CTab
{
public:
	void Setup();
	CGroupBox1 EzTextColors;
	// Main Menu
	CGroupBox ColorMain;
	CGroupBox Radar;
	CSlider   RMainMenuA;
	CSlider   GMainMenuA;
	CSlider   BMainMenuA;
	CSlider   AMainMenuA;
	CSlider   RMainMenuB;
	CSlider   GMainMenuB;
	CSlider   BMainMenuB;
	CSlider   AMainMenuB;
	CSlider TColorVisR;
	CSlider TColorVisG;
	CSlider TColorVisB;
	CSlider CTColorVisR;
	CSlider CTColorVisG;
	CSlider CTColorVisB;
	CSlider TColorNoVisR;
	CSlider TColorNoVisG;
	CSlider TColorNoVisB;
	CSlider CTColorNoVisR;
	CSlider CTColorNoVisG;
	CSlider CTColorNoVisB;
	// Menu Text
	CGroupBox ColorText;
	CSlider   RMenuText;
	CSlider   GMenuText;
	CSlider   BMenuText;





	//NoScope
	CGroupBox NoScope;
	CSlider NoScopeR;
	CSlider NoScopeG;
	CSlider NoScopeB;



	//FillBox
	CSlider   FillBoxRed;
	CSlider   FillBoxGreen;
	CSlider   FillBoxBlue;
	CSlider   FillBoxAlpha;

	CGroupBox ColorBulletTrace;
	CSlider   RBT;
	CSlider   GBT;
	CSlider   BBT;
	// Inner Window
	CGroupBox ColorInner;
	CSlider   RInner;
	CSlider   GInner;
	CSlider   BInner;
	CSlider   AInner;

	// Tabs
	CGroupBox ColorTabs;
	CSlider   RTabs;
	CSlider   GTabs;
	CSlider   BTabs;
	
};

// Elements that can be placed anywhere in any tabs, should be decleared here.
class AyyWareWindow : public CWindow
{
public:
	void Setup();

	CRageBotTab RageBotTab;
	CLegitBotTab LegitBotTab;
	CVisualTab VisualsTab;
	CMiscTab MiscTab;
	CSkinTab SkinTab;
	CGUITab GUITab;

	CButton SaveButton;
	CButton LoadButton;
	CButton UnloadButton;
	CButton UpdateWalls;
	CComboBox ConfigBox;
};

namespace Menu
{
	void CheckVersionsCSGO();
	void SetupMenu();
	void DoUIFrame();

	extern AyyWareWindow Window;
};