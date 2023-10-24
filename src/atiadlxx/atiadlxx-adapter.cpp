#include "adlx_common.h"


extern "C"
{
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
}
