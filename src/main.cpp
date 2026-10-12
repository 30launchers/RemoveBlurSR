#include "pch.h"
#include "MinHookManager.h"
#include "PatternScanner.hpp"
#include <atomic>
#include <vector>

constexpr const char* SET_VISIBLE_PATTERN = "41 57 41 56 56 57 55 53 48 83 EC 28 45 89 CE 44 89 C5 41 89 D7 48 89 CE";
constexpr const char* APPLY_DITHER_PATTERN ="56 53 48 83 EC 38 0F 29 74 24 20 44 89 C3 0F 28 F1 48 89 CE 80 3D ?? ??";

// Types
using ApplyDither_t = void(__fastcall*)(void* pThis, float value, bool flag);
using SetVisible_t = void(__fastcall*)(void* pThis, bool visible, bool force, int reason);

// Globals
SetVisible_t g_originalSetVisible = nullptr;
static std::atomic<bool> g_enabled{ false };

// Dynamic Dither Hook List
struct DitherHookEntry
{
    uintptr_t address;
    ApplyDither_t original;
};

static std::vector<DitherHookEntry> g_ditherHooks;

// Shared Dither Handler
void HandleApplyDither(ApplyDither_t original, void* pThis, float value, bool flag)
{
    if (g_enabled.load())
    {
        value = 1.0f;
    }
    original(pThis, value, flag);
}

// Generate Dither Hooks
#define MAKE_DITHER_HOOK(idx) \
void __fastcall hk_ApplyDither_##idx(void* pThis, float value, bool flag) \
{ \
    HandleApplyDither( \
        g_ditherHooks[idx].original, \
        pThis, \
        value, \
        flag \
    ); \
}


// 预留32个Dither Hook槽
MAKE_DITHER_HOOK(0)
MAKE_DITHER_HOOK(1)
MAKE_DITHER_HOOK(2)
MAKE_DITHER_HOOK(3)
MAKE_DITHER_HOOK(4)
MAKE_DITHER_HOOK(5)
MAKE_DITHER_HOOK(6)
MAKE_DITHER_HOOK(7)
MAKE_DITHER_HOOK(8)
MAKE_DITHER_HOOK(9)
MAKE_DITHER_HOOK(10)
MAKE_DITHER_HOOK(11)
MAKE_DITHER_HOOK(12)
MAKE_DITHER_HOOK(13)
MAKE_DITHER_HOOK(14)
MAKE_DITHER_HOOK(15)
MAKE_DITHER_HOOK(16)
MAKE_DITHER_HOOK(17)
MAKE_DITHER_HOOK(18)
MAKE_DITHER_HOOK(19)
MAKE_DITHER_HOOK(20)
MAKE_DITHER_HOOK(21)
MAKE_DITHER_HOOK(22)
MAKE_DITHER_HOOK(23)
MAKE_DITHER_HOOK(24)
MAKE_DITHER_HOOK(25)
MAKE_DITHER_HOOK(26)
MAKE_DITHER_HOOK(27)
MAKE_DITHER_HOOK(28)
MAKE_DITHER_HOOK(29)
MAKE_DITHER_HOOK(30)
MAKE_DITHER_HOOK(31)


// 预留32个Dither Hook槽
static void* g_ditherDetours[] =
{
    hk_ApplyDither_0,
    hk_ApplyDither_1,
    hk_ApplyDither_2,
    hk_ApplyDither_3,
    hk_ApplyDither_4,
    hk_ApplyDither_5,
    hk_ApplyDither_6,
    hk_ApplyDither_7,
    hk_ApplyDither_8,
    hk_ApplyDither_9,
    hk_ApplyDither_10,
    hk_ApplyDither_11,
    hk_ApplyDither_12,
    hk_ApplyDither_13,
    hk_ApplyDither_14,
    hk_ApplyDither_15,
    hk_ApplyDither_16,
    hk_ApplyDither_17,
    hk_ApplyDither_18,
    hk_ApplyDither_19,
    hk_ApplyDither_20,
    hk_ApplyDither_21,
    hk_ApplyDither_22,
    hk_ApplyDither_23,
    hk_ApplyDither_24,
    hk_ApplyDither_25,
    hk_ApplyDither_26,
    hk_ApplyDither_27,
    hk_ApplyDither_28,
    hk_ApplyDither_29,
    hk_ApplyDither_30,
    hk_ApplyDither_31,
};


// Visible Hook
void __fastcall hk_SetVisible(void* pThis, bool visible, bool force, int reason)
{
    LOG("SetVisible visible=%d force=%d reason=%d", visible, force, reason);
    if (g_enabled.load())
    {
        if (reason == 1 && visible == false && force == false)
        {
            visible = true;
        }
    }

    g_originalSetVisible(pThis, visible, force, reason);
}

static std::vector<void*> g_installedHooks;
// Hook Helpers
bool InstallHookAddress(uintptr_t address, void* detour, void** original)
{
    if (!address)
        return false;

    bool result = MinHookManager::Add(reinterpret_cast<void*>(address), detour, original);

    if(result)
    {
        g_installedHooks.push_back(reinterpret_cast<void*>(address));
    }

    return result;
}

void RemoveAllHooks()
{
    LOG("Removing hooks...");

    for(auto addr : g_installedHooks)
    {
        MinHookManager::Remove(addr);
    }

    g_installedHooks.clear();

    LOG("All hooks removed");
}

// Main Logic
void Run()
{
    LOG("Installing hooks...");
    bool allOk = true;

    // ApplyDither Pattern Scan
    auto ditherResults = PatternScanner::MultipleScan(APPLY_DITHER_PATTERN);
    LOG("ApplyDither found: %zu", ditherResults.size());

    for (size_t i = 0; i < ditherResults.size(); i++)
    {

        if (i >= std::size(g_ditherDetours))
        {
            ERR("Too many ApplyDither results!");
            break;
        }

        DitherHookEntry entry{};
        entry.address = ditherResults[i];
        entry.original = nullptr;
        g_ditherHooks.push_back(entry);

        bool ok = InstallHookAddress(entry.address, g_ditherDetours[i], reinterpret_cast<void**>(&g_ditherHooks.back().original));

        if(ok)
        {
            LOG("[OK] ApplyDither %zu 0x%llX", i, entry.address);
        }
        else
        {
            ERR("[FAILED] ApplyDither %zu",i);
            allOk=false;
        }
    }

    // SetVisible Pattern Scan
    auto visibleResults = PatternScanner::MultipleScan(SET_VISIBLE_PATTERN);

    if(!visibleResults.empty())
    {

        uintptr_t addr = visibleResults[0];
        LOG( "[FOUND] SetVisible @ 0x%llX", addr);
        bool ok = InstallHookAddress(addr, reinterpret_cast<void*>(hk_SetVisible), reinterpret_cast<void**>(&g_originalSetVisible));

        if(!ok)
        {
            ERR("[FAILED] SetVisible hook");
            allOk=false;
        }

    }
    else
    {
        ERR("[FAILED] SetVisible pattern not found");
        allOk=false;
    }

    if(allOk)
    {
        LOG("[SUCCESS] All hooks installed");
        LOG("Press F8 to toggle");
    }
    else
    {
        ERR("[WARNING] Some hooks failed");
    }
}

// Toggle Loop
void ToggleLoop()
{
    bool prev=false;

    while(true)
    {
        Sleep(100);

        bool pressed = (GetAsyncKeyState(VK_F8)&0x8000) != 0;
        if(pressed&&!prev)
        {
            bool state = !g_enabled.load();
            g_enabled.store(state);
            LOG( "[F8] Remove Blur: %s", state?"ON":"OFF");
        }

        prev=pressed;
    }
}

// Thread Entry
DWORD WINAPI MainThread(LPVOID)
{
    AllocConsole();

    FILE* f;
    freopen_s( &f, "CONOUT$", "w", stdout);
    LOG("DLL Loaded");
    Sleep(3000);
    Run();
    ToggleLoop();
    return 0;
}

// DllMain
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch(reason)
    {
        case DLL_PROCESS_ATTACH:
        {
            DisableThreadLibraryCalls(hModule);
            HANDLE hThread = CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
            if(hThread)
                CloseHandle(hThread);

            break;
        }
        case DLL_PROCESS_DETACH:
        {
            LOG("DLL unloading");
            RemoveAllHooks();
            break;
        }
    }

    return TRUE;
}