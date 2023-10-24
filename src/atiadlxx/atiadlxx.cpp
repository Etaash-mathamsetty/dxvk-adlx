#include "../adlx_private.h"
#include "../dxvk/dxvk_interfaces.h"
#include "../util/com_pointer.h"
#include "../../version.h"

static auto adlMutex = std::mutex{};

/*
#define ADL_LOCK() std::unique_lock<std::mutex> _lock____ \
  = (adl_context->threading_model == ADL_THREADING_LOCKED) ? \
  std::unique_lock<std::mutex>(adlMutex) : std::unique_lock<std::mutex>() \
*/

//no-op for now, since no known applications to test with
#define ADL_LOCK() do { } while(0)

//global adl1 context, struct def for adl context
struct ADL_CONTEXT
{
    ADLThreadingModel threading_model;
    ADL_MAIN_MALLOC_CALLBACK adl_malloc;
    int enum_connected_adapters;
    dxvk::Com<IDXGIFactory> dxgi_factory;
    VkInstance vk_instance;
    PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr;
} global_adl_context;

extern "C"
{
    using namespace dxvk;

    void* __stdcall ADL2_Main_Control_GetProcAddress(ADL_CONTEXT_HANDLE context, void *module, char* proc_name)
    {
        //printf("TRACE: ADL2_Main_Control_GetProcAddress: %p %s\n", module, proc_name);
        return (void*)GetProcAddress((HMODULE)module, proc_name);;
    }

    int __stdcall ADL2_Main_Control_Refresh(ADL_CONTEXT_HANDLE context)
    {
        ADL_CONTEXT *adl_context = (ADL_CONTEXT*) context;
        ADL_LOCK();
        if(adl_context->enum_connected_adapters == 0)
        {
            printf("FIXME: ADL2_Main_Control_Refresh enum_connected_adapters == 0\n");
        }

        //do the refreshing here


        return ADL_OK;
    }

    int __stdcall ADL2_Adapter_NumberOfAdapters_Get(ADL_CONTEXT_HANDLE context, int* num_adapters)
    {
        ADL_CONTEXT *adl_context = (ADL_CONTEXT*) context;
        ADL_LOCK();

        if(!num_adapters)
            return ADL_ERR_INVALID_PARAM;

        PFN_vkEnumeratePhysicalDevices func = (PFN_vkEnumeratePhysicalDevices)adl_context->vk_get_instance_proc_addr(
                                                                                adl_context->vk_instance, "vkEnumeratePhysicalDevices");
        if(!func)
        {
            printf("ERROR: ADL2_Adapter_NumberOfAdapters_Get vkEnumeratePhysicalDevices not found\n");
            return ADL_ERR;
        }

        uint32_t num_adapters_vk = 0;
        func(adl_context->vk_instance, &num_adapters_vk, nullptr);

        *num_adapters = num_adapters_vk;

        return ADL_OK;
    }

    int __stdcall ADL2_Main_ControlX3_Create(ADL_MAIN_MALLOC_CALLBACK callback, int iEnumConnectedAdapters, ADL_CONTEXT_HANDLE* context,
                                                ADLThreadingModel threadingModel, int adlCreateOptions)
    {
        ADL_CONTEXT *adl_context = nullptr;

        if(!callback || !context)
            return ADL_ERR_INVALID_PARAM;

        if(adlCreateOptions != 0)
        {
            printf("FIXME: ADL2_Main_Control_Create Unimplemented adlCreateOptions: %x\n", adlCreateOptions);
        }

        adl_context = (ADL_CONTEXT*) calloc(1, sizeof(ADL_CONTEXT));
        if(!adl_context)
            return ADL_ERR;

        *context = (ADL_CONTEXT_HANDLE) adl_context;
        adl_context->threading_model = threadingModel;
        adl_context->adl_malloc = callback;
        adl_context->enum_connected_adapters = iEnumConnectedAdapters;
        if(FAILED(CreateDXGIFactory(IID_PPV_ARGS(&adl_context->dxgi_factory))))
        {
            printf("ERROR: ADL2_Main_Control_Create CreateDXGIFactory failed\n");
            return ADL_ERR;
        }

        Com<IDXGIVkInteropFactory> interopFactory;
        if(FAILED(adl_context->dxgi_factory->QueryInterface(IID_PPV_ARGS(&interopFactory))))
        {
            printf("ERROR: Failed to query: IDXGIVkInteropFactory. Make sure you are using dxvk's dxgi.dll!\n");
            return ADL_ERR;
        }

        interopFactory->GetVulkanInstance(&adl_context->vk_instance, &adl_context->vk_get_instance_proc_addr);

        ADL2_Main_Control_Refresh(*context);

        return ADL_OK;
    }

    int __stdcall ADL2_Main_ControlX2_Create(ADL_MAIN_MALLOC_CALLBACK callback, int iEnumConnectedAdapters, ADL_CONTEXT_HANDLE* context,
                                                ADLThreadingModel threadingModel)
    {
        return ADL2_Main_ControlX3_Create(callback, iEnumConnectedAdapters, context, threadingModel, 0);
    }

    int __stdcall ADL2_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK callback, int iEnumConnectedAdapters, ADL_CONTEXT_HANDLE* context)
    {
        return ADL2_Main_ControlX2_Create(callback, iEnumConnectedAdapters, context, ADL_THREADING_UNLOCKED);
    }

    int __stdcall ADL2_Main_Control_Destroy(ADL_CONTEXT_HANDLE context)
    {
        ADL_CONTEXT *adl_context = (ADL_CONTEXT*) context;
        ADL_LOCK();
        //not freeing dxgi factory since it is basically a global singleton
        free(adl_context);
        return ADL_OK;
    }

}
