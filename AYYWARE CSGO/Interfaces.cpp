/*
Syn's AyyWare Framework 2015
*/

#include "Interfaces.h"
#include "Utilities.h"
#include "UTIL Functions.h"
#include "DLLMain.h"
#include "IPrediction.h"
#include "igameeventmanager.h"
#define strenc( s ) ( s )

//SDK Specific Definitions
typedef void* (__cdecl* CreateInterface_t)(const char*, int*);
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

//Some globals for later
CreateInterface_t EngineFactory = NULL; // These are used to store the individual
CreateInterface_t ClientFactory = NULL; //  CreateInterface functions for each game
CreateInterface_t VGUISurfaceFactory = NULL; //  dll that we need access to. Can call
CreateInterface_t VGUI2Factory = NULL; //  them to recieve pointers to game classes.
CreateInterface_t MatFactory = NULL;
CreateInterface_t PhysFactory = NULL;
CreateInterface_t StdFactory = NULL;

void Interfaces::Initialise()
{
	//Get function pointers to the CreateInterface function of each module
	EngineFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::Engine, "CreateInterface");
	ClientFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::Client, "CreateInterface");
	VGUI2Factory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::VGUI2, "CreateInterface");
	VGUISurfaceFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::VGUISurface, "CreateInterface");
	MatFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::Material, "CreateInterface");
	PhysFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::VPhysics, "CreateInterface");
	StdFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::Stdlib, "CreateInterface");

	//Get the interface names regardless of their version number by scanning for each string
	char* CHLClientInterfaceName = (char*)Utilities::Memory::FindTextPattern("client.dll", "VClient018");
	char* VGUI2PanelsInterfaceName = (char*)Utilities::Memory::FindTextPattern("vgui2.dll", "VGUI_Panel009");
	char* VGUISurfaceInterfaceName = (char*)Utilities::Memory::FindTextPattern("vguimatsurface.dll", "VGUI_Surface031");
	char* EntityListInterfaceName = (char*)Utilities::Memory::FindTextPattern("client.dll", "VClientEntityList003");
	char* EngineDebugThingInterface = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VDebugOverlay004");
	char* EngineClientInterfaceName = (char*)Utilities::Memory::FindTextPattern("engine.dll","VEngineClient014");
	char* ClientPredictionInterface = (char*)Utilities::Memory::FindTextPattern("client.dll", "VClientPrediction0");
	char* MatSystemInterfaceName = (char*)Utilities::Memory::FindTextPattern("materialsystem.dll", "VMaterialSystem080");
	char* EngineRenderViewInterface = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VEngineRenderView014");
	char* EngineModelRenderInterface = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VEngineModel016");
	char* EngineModelInfoInterface = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VModelInfoClient004");
	char* EngineTraceInterfaceName = (char*)Utilities::Memory::FindTextPattern("engine.dll", "EngineTraceClient004");
	char* PhysPropsInterfaces = (char*)Utilities::Memory::FindTextPattern("vphysics.dll", "VPhysicsSurfaceProps001");
	char* VEngineCvarName = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VEngineCvar00");


	GameMovement = reinterpret_cast<IGameMovement*>(Utilities::Memory::FindTextPattern("client.dll", "GameMovement"));
	GamePrediction = reinterpret_cast<IPrediction*>(Utilities::Memory::FindTextPattern("client.dll", "VClientPrediction"));
	GameEventManager = (IGameEventManager2*)EngineFactory("GAMEEVENTSMANAGER002", NULL);

	// Use the factory function pointers along with the interface versions to grab
	//  pointers to the interface
	Client = (IBaseClientDLL*)ClientFactory(CHLClientInterfaceName, NULL);
	Engine = (IVEngineClient*)EngineFactory(EngineClientInterfaceName, NULL);
	Panels = (IPanel*)VGUI2Factory(VGUI2PanelsInterfaceName, NULL);
	Surface = (ISurface*)VGUISurfaceFactory(VGUISurfaceInterfaceName, NULL);
	EntList = (IClientEntityList*)ClientFactory(EntityListInterfaceName, NULL);
	DebugOverlay = (IVDebugOverlay*)EngineFactory(EngineDebugThingInterface, NULL);
	Prediction = (DWORD*)ClientFactory(ClientPredictionInterface, NULL);
	MaterialSystem = (CMaterialSystem*)MatFactory(MatSystemInterfaceName, NULL);
	RenderView = (CVRenderView*)EngineFactory(EngineRenderViewInterface, NULL);
	ModelRender = (IVModelRender*)EngineFactory(EngineModelRenderInterface, NULL);
	ModelInfo = (CModelInfo*)EngineFactory(EngineModelInfoInterface, NULL);
	Trace = (IEngineTrace*)EngineFactory(EngineTraceInterfaceName, NULL);
	PhysProps = (IPhysicsSurfaceProps*)PhysFactory(PhysPropsInterfaces, NULL);
	CVar = (ICVar*)StdFactory(VEngineCvarName, NULL);

	// Get ClientMode Pointer
	ClientMode = **(IClientModeShared***)((*(uintptr_t**)Client)[10] + 0x5);

	// Search through the first entry of the Client VTable
	// The initializer contains a pointer to the 'GlobalsVariables' Table
	PDWORD pdwClient = (PDWORD)*(PDWORD)Client;
	DWORD dwInitAddr = (DWORD)(pdwClient[0]);

	for (DWORD dwIter = 0; dwIter <= 0xFF; dwIter++)
	{
		if (*(PBYTE)(dwInitAddr + dwIter - 1) == 0x08 && *(PBYTE)(dwInitAddr + dwIter) == 0xA3)
		{
			// GlobalVars Signature
			Globals = **reinterpret_cast<CGlobalVarsBase***>((*reinterpret_cast<DWORD**>(Client))[0] + 0x1B);
			break;
		}
	}

	PDWORD pdwClientVMT = *(PDWORD*)Client;

	// CInput Signature
	pInput = *(CInput**)((*(DWORD**)Client)[15] + 0x1);

	Utilities::Log("Interfaces Ready");
}

// Namespace to contain all the valve interfaces
namespace Interfaces
{
	IBaseClientDLL* Client;
	IVEngineClient* Engine;
	IPanel* Panels;
	IClientEntityList* EntList;
	ISurface* Surface;
	IVDebugOverlay* DebugOverlay;
	IClientModeShared *ClientMode;
	CGlobalVarsBase *Globals;
	DWORD *Prediction;
	CMaterialSystem* MaterialSystem;
	CVRenderView* RenderView;
	IVModelRender* ModelRender;
	CModelInfo* ModelInfo;
	IEngineTrace* Trace;
	IPhysicsSurfaceProps* PhysProps;
	ICVar *CVar;
	CInput* pInput;
	IGameEventManager2 *Interfaces::GameEventManager;
	IMoveHelper* MoveHelper;
	IGameMovement* GameMovement;
	IPrediction* GamePrediction;
};