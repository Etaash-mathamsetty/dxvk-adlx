#include "../adlx_common.h"


extern "C"
{
    using namespace dxvk;

    static __stdcall int count_monitors(HMONITOR monitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
    {
        int* num_monitors = (int*)dwData;
        (*num_monitors)++;
        return TRUE;
    }

    void* DLLEXPORT ADL2_Main_Control_GetProcAddress(ADL_CONTEXT_HANDLE context, void *module, char* proc_name)
    {
        //printf("TRACE: ADL2_Main_Control_GetProcAddress: %p %s\n", module, proc_name);
        return (void*)GetProcAddress((HMODULE)module, proc_name);
    }

    void* DLLEXPORT ADL_Main_Control_GetProcAddress(void *module, char* proc_name)
    {
        return ADL2_Main_Control_GetProcAddress((ADL_CONTEXT_HANDLE)&global_adl_context, module, proc_name);
    }

    int DLLEXPORT ADL2_Main_Control_Refresh(ADL_CONTEXT_HANDLE context)
    {
        //no-op
        return ADL_OK;
    }

    int DLLEXPORT ADL_Main_Control_Refresh()
    {
        return ADL2_Main_Control_Refresh((ADL_CONTEXT_HANDLE)&global_adl_context);
    }

    int DLLEXPORT ADL2_Main_ControlX3_Create(ADL_MAIN_MALLOC_CALLBACK callback, int iEnumConnectedAdapters, ADL_CONTEXT_HANDLE* context,
                                                ADLThreadingModel threadingModel, int adlCreateOptions)
    {
        ADL_CONTEXT *adl_context = nullptr;

        HMODULE ntdll = GetModuleHandleA("ntdll.dll");
        if(!ntdll)
        {
            return ADL_ERR;
        }

        PFN_wineDbgOutput wine_dbg = reinterpret_cast<PFN_wineDbgOutput>(reinterpret_cast<void*>(GetProcAddress(ntdll, "__wine_dbg_output")));
        if(!wine_dbg)
        {
            return ADL_ERR;
        }

        if(!callback || !context)
            return ADL_ERR_INVALID_PARAM;

        ///wine_dbg("HELLOOOOOOO?????????????????????????????\n");

        adl_context = (ADL_CONTEXT*) calloc(1, sizeof(ADL_CONTEXT));
        if(!adl_context)
            return ADL_ERR;

        *context = (ADL_CONTEXT_HANDLE) adl_context;
        adl_context->threading_model = threadingModel;
        adl_context->adl_malloc = callback;
        adl_context->enum_connected_adapters = iEnumConnectedAdapters;
        wine_dbg = wine_dbg;
        log_file = std::ofstream("dxvk-adlx.log");

        EnumDisplayMonitors(NULL, NULL, count_monitors, (LPARAM)&adl_context->monitor_count);

        print( "DXVK-ADLX: " + std::string(DXVK_ADLX_VERSION) + "\n");

        if(adlCreateOptions != 0)
        {
            print( std::string("FIXME: ADL2_Main_Control_Create Unimplemented adlCreateOptions: ") + std::to_string(adlCreateOptions) + "\n");
        }

        if(FAILED(CreateDXGIFactory(IID_PPV_ARGS(&adl_context->dxgi_factory))))
        {
            print( "ERROR: ADL2_Main_Control_Create CreateDXGIFactory failed\n");
            return ADL_ERR;
        }

        Com<IDXGIVkInteropFactory> interopFactory;
        if(FAILED(adl_context->dxgi_factory->QueryInterface(IID_PPV_ARGS(&interopFactory))))
        {
            print( "ERROR: Failed to query: IDXGIVkInteropFactory. Make sure you are using dxvk's dxgi.dll!\n");
            return ADL_ERR;
        }

        interopFactory->GetVulkanInstance(&adl_context->vk_instance, &adl_context->vk_get_instance_proc_addr);

        ADL2_Main_Control_Refresh(*context);

        print( "TRACE: ADL2_Main_ControlX3_Create: vk_instance: " + std::to_string((uint64_t)adl_context->vk_instance) + "\n");
        print( "Using ADL 2.0\n");

        return ADL_OK;
    }

    int DLLEXPORT ADL2_Main_ControlX2_Create(ADL_MAIN_MALLOC_CALLBACK callback, int iEnumConnectedAdapters, ADL_CONTEXT_HANDLE* context,
                                                ADLThreadingModel threadingModel)
    {
        return ADL2_Main_ControlX3_Create(callback, iEnumConnectedAdapters, context, threadingModel, 0);
    }

    int DLLEXPORT ADL2_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK callback, int iEnumConnectedAdapters, ADL_CONTEXT_HANDLE* context)
    {
        return ADL2_Main_ControlX2_Create(callback, iEnumConnectedAdapters, context, ADL_THREADING_UNLOCKED);
    }

    int DLLEXPORT ADL2_Main_Control_Destroy(ADL_CONTEXT_HANDLE context)
    {
        ADL_CONTEXT *adl_context = (ADL_CONTEXT*) context;
        ADL_LOCK();
        //not freeing dxgi factory since it is basically a global singleton
        free(adl_context);
        log_file.close();
        return ADL_OK;
    }

    int DLLEXPORT ADL_Main_ControlX2_Create(ADL_MAIN_MALLOC_CALLBACK callback, int iEnumConnectedAdapters, ADLThreadingModel threadingModel)
    {
        ADL_CONTEXT *adl_context = &global_adl_context;

        HMODULE ntdll = GetModuleHandleA("ntdll.dll");
        if(!ntdll)
        {
            //print( "ERROR: ADL2_Main_Control_Create failed to get ntdll\n");
            return ADL_ERR;
        }

        PFN_wineDbgOutput wine_dbg = reinterpret_cast<PFN_wineDbgOutput>(reinterpret_cast<void*>(GetProcAddress(ntdll, "__wine_dbg_output")));
        if(!wine_dbg)
        {
           // print( "ERROR: ADL2_Main_Control_Create failed to get __wine_dbg_output\n");
            return ADL_ERR;
        }

        if(!callback)
            return ADL_ERR_INVALID_PARAM;

        adl_context->threading_model = threadingModel;
        adl_context->adl_malloc = callback;
        adl_context->enum_connected_adapters = iEnumConnectedAdapters;
        adl_context->is_adl1 = true;
        wine_dbg = wine_dbg;
        log_file = std::ofstream("dxvk-adlx.log");

        EnumDisplayMonitors(NULL, NULL, count_monitors, (LPARAM)&adl_context->monitor_count);

        print( "DXVK-ADLX: " + std::string(DXVK_ADLX_VERSION) + "\n");

        if(FAILED(CreateDXGIFactory(IID_PPV_ARGS(&adl_context->dxgi_factory))))
        {
            print( "ERROR: ADL_Main_ControlX2_Create CreateDXGIFactory failed\n");
            return ADL_ERR;
        }

        Com<IDXGIVkInteropFactory> interopFactory;
        if(FAILED(adl_context->dxgi_factory->QueryInterface(IID_PPV_ARGS(&interopFactory))))
        {
            print( "ERROR: Failed to query: IDXGIVkInteropFactory. Make sure you are using dxvk's dxgi.dll!\n");
            return ADL_ERR;
        }

        interopFactory->GetVulkanInstance(&adl_context->vk_instance, &adl_context->vk_get_instance_proc_addr);

        ADL_Main_Control_Refresh();

        print( "TRACE: ADL_Main_ControlX2_Create: vk_instance: " + std::to_string((uint64_t)adl_context->vk_instance) + "\n");
        print( "Using ADL 1.0\n");

        return ADL_OK;
    }

    int DLLEXPORT ADL_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK callback, int iEnumConnectedAdapters)
    {
        return ADL_Main_ControlX2_Create(callback, iEnumConnectedAdapters, ADL_THREADING_UNLOCKED);
    }

    int DLLEXPORT ADL_Main_Control_Destroy()
    {
        log_file.close();
        return ADL_OK;
    }
}
