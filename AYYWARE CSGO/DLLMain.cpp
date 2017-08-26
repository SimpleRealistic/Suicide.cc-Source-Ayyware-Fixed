// General shit
#include "DLLMain.h"
#include "Utilities.h"

// Injection stuff
#include "INJ/ReflectiveLoader.h"

// Stuff to initialise
#include "Offsets.h"
#include "Interfaces.h"
#include "Hooks.h"
#include "RenderManager.h"
#include "Hacks.h"
#include "Menu.h"
#include "AntiAntiAim.h"
#include "xor.h"
#include "Dumping.h"
#include "AntiLeak.h"

void CAntiLeak::ErasePE()
{

	char *pBaseAddr = (char*)GetModuleHandle(NULL);
	DWORD dwOldProtect = 0;
	VirtualProtect(pBaseAddr, 4096, PAGE_READWRITE, &dwOldProtect);
	ZeroMemory(pBaseAddr, 4096);
	VirtualProtect(pBaseAddr, 4096, dwOldProtect, &dwOldProtect);

}

bool DoUnload;

HINSTANCE HThisModule;

UCHAR szFileSys[255], szVolNameBuff[255];
DWORD dwMFL, dwSysFlags;
DWORD dwSerial;


					   // Our thread we use to setup everything we need
					   // Everything appart from code in hooks get's called from inside 
					   // here.


int InitialThread(LPVOID lpArguments)
{
	// MessageBox(NULL, "Hack injected successfully.", "INTERX 2k16", MB_OK);
/*	HANDLE hOlly = FindWindow(TEXT("OLLYDBG"), NULL);
	HANDLE hWinDbg = FindWindow(TEXT("WinDbgFrameClass"), NULL);
	HANDLE hScylla1 = FindWindow(NULL, TEXT("Scylla x86 v0.9.7c"));
	HANDLE hScylla2 = FindWindow(NULL, TEXT("Scylla x64 v0.9.7c"));
	HANDLE x32_dbg = FindWindow(NULL, TEXT("x32_dbg"));
	HANDLE x64_dbg = FindWindow(NULL, TEXT("x64_dbg"));
	HANDLE IDA = FindWindow(NULL, TEXT("IDA"));

	if (IsDebuggerPresent())
		ExitProcess(0);
	if (IsDbgPresentPrefixCheck())
		ExitProcess(0);
	if (Int2DCheck())
		ExitProcess(0);
	if (hOlly)
		ExitProcess(0);
	if (hWinDbg)
		ExitProcess(0);
	if (hScylla1)
		ExitProcess(0);
	if (hScylla2)
		ExitProcess(0);
	if (x32_dbg)
		ExitProcess(0);
	if (x64_dbg)
		ExitProcess(0);

	OutputDebugString(TEXT("%s%s%s%s%s%s%s%s%s%s%s")TEXT("%s%s%s%s%s%s%s%s%s%s%s%s%s")TEXT("%s%s%s%s%s%s%s%s%s%s%s%s%s")TEXT("%s%s%s%s%s%s%s%s%s%s%s%s%s"));*/

	Offsets::Initialise(); // Set our VMT offsets and do any pattern scans
	Interfaces::Initialise(); // Get pointers to the valve classes
	NetVar.RetrieveClasses(); // Setup our NetVar manager (thanks shad0w bby)
	Render::Initialise();
	Hacks::SetupHacks();





	Menu::CheckVersionsCSGO();





	
	Hooks::Initialise();
	ApplyAAAHooks();
	// AntiLeak->ErasePE();

	//get the current process' name, and exit if it is the target process

	//int i = 0; i < 3000; i += 10;
	//Beep(i, 300);

	//int i2 = 3000; i2 > 0; i2 -= 10;
	//Beep(i2, 300);


	//if the thread fails to detach from the game, kill the host process
	// While our cheat is running
	while (DoUnload == false)
	{
		Sleep(1000);
	}

	Hooks::UndoHooks();
	Sleep(2000); // Make sure none of our hooks are running
	FreeLibraryAndExitThread(HThisModule, 0);

	return 0;
}

// Entry point for da pussy
bool _stdcall DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		if (!GetModuleHandle(/*client.dll*/XorStr<0xFE, 11, 0x528052E7>("\x9D\x93\x69\x64\x6C\x77\x2A\x61\x6A\x6B" + 0x528052E7).s))
			Sleep(300);
		if (!GetModuleHandle(/*engine.dll*/XorStr<0xD8, 11, 0x190E3235>("\xBD\xB7\xBD\xB2\xB2\xB8\xF0\xBB\x8C\x8D" + 0x190E3235).s))
			Sleep(300);

		HideThread(0);
		DisableThreadLibraryCalls(hinst);
		if (GetModuleHandleA(/*csgo.exe*/XorStr<0xCE, 9, 0xF6281E21>("\xAD\xBC\xB7\xBE\xFC\xB6\xAC\xB0" + 0xF6281E21).s))
		{
			//Sleep(1000);
			HThisModule = hinst;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InitialThread, NULL, NULL, NULL);
		}
		else if (!GetModuleHandleA(/*csgo.exe*/XorStr<0xCE, 9, 0xF6281E21>("\xAD\xBC\xB7\xBE\xFC\xB6\xAC\xB0" + 0xF6281E21).s))
		{
			MessageBox(NULL, XorStr<0x3A, 78, 0xD5A19EB9>("\x6E\x53\x55\x4E\x1E\x5C\x21\x2F\x2C\x2C\x30\x65\x24\x22\x68\x20\x24\x21\x29\x2E\x3A\x2A\x34\x71\x3B\x3D\x74\x34\x38\x38\x2C\x31\x3F\x29\x7C\x2D\x2C\x30\x03\x04\x11\x10\x4A\x4B\x48\x6D\x38\x05\x0F\x0A\x1F\x08\x4E\x00\x00\x14\x1C\x53\x48\x16\x05\x10\x17\x57\x1F\x03\x19\x43\x5E\x0B\xEF\xA1\xF5\xEC\xF6\xEE\xA8" + 0xD5A19EB9).s
				,XorStr<0xB2, 25, 0x7EF30988>("\xE9\xF0\xE9\x87\x86\x86\x8E\x99\x97\x9B\xF9\xEF\xEC\xF0\x92\xE1\xEF\xE3\x8D\x8B\x92\x82\x9A\x91" + 0x7EF30988).s
				, MB_OK);
			exit(0);
			return TRUE;
		}
	}
	return TRUE;
}