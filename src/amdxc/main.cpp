#include "../adlx_common.h"

static HINSTANCE g_hinstDLL = nullptr;

extern "C"
{
    HRESULT DLLEXPORT AmdExtD3DCreateInterface(
        IUnknown*   pOuter,     ///< [in] object on which to base this new interface; usually a D3D device
        REFIID      riid,       ///< ID of the requested interface
        void**      ppvObject)  ///< [out] The result interface object
    {
        print("AmdExtD3DCreateInterface " + iid_to_string(riid) + " stub!\n");
        return E_NOTIMPL;
    }

    HINSTANCE DLLEXPORT AmdGetDxcModuleHandle()
    {
        print("TRACE: AmdGetDxcModuleHandle(void) ret: " + std::to_string((uint64_t)g_hinstDLL) + "\n");
        return g_hinstDLL;
    }

    bool DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
    {
        if(fdwReason == DLL_PROCESS_ATTACH)
        {
            g_hinstDLL = hinstDLL;
            log_file = std::ofstream("dxvk-adlx-amdxc.log");
            HMODULE mod = GetModuleHandleA("ntdll.dll");
            wine_dbg = reinterpret_cast<PFN_wineDbgOutput>(reinterpret_cast<void*>(GetProcAddress(mod, "__wine_dbg_output")));
            print("TRACE: DllMain (amdxc)!");
        }
        return true;
    }
}
