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

    int __stdcall ADL2_Adapter_AdapterInfo_Get(ADL_CONTEXT_HANDLE context, AdapterInfo *info_arr, int size)
    {
        ADL_CONTEXT *adl_context = (ADL_CONTEXT*) context;
        ADL_LOCK();

        if(!info_arr)
            return ADL_ERR_INVALID_PARAM;

        PFN_vkEnumeratePhysicalDevices func = (PFN_vkEnumeratePhysicalDevices)adl_context->vk_get_instance_proc_addr(
                                                                                adl_context->vk_instance, "vkEnumeratePhysicalDevices");
        PFN_vkGetPhysicalDeviceProperties func2 = (PFN_vkGetPhysicalDeviceProperties)adl_context->vk_get_instance_proc_addr(
                                                                                adl_context->vk_instance, "vkGetPhysicalDeviceProperties");
        if(!func || !func2)
        {
            printf("ERROR: ADL2_Adapter_AdapterInfo_Get failed to get \n");
            return ADL_ERR;
        }

        uint32_t num_adapters_vk = 0;
        func(adl_context->vk_instance, &num_adapters_vk, nullptr);

        size /= sizeof(AdapterInfo);
        uint32_t num_iter_adapters = std::min(num_adapters_vk, (uint32_t)size);

        VkPhysicalDevice *adapters = (VkPhysicalDevice*)calloc(num_adapters_vk, sizeof(VkPhysicalDevice));
        if(!adapters)
        {
            printf("ERROR: ADL2_Adapter_AdapterInfo_Get calloc failed\n");
            return ADL_ERR;
        }

        func(adl_context->vk_instance, &num_adapters_vk, adapters);

        for(uint32_t i = 0; i < num_iter_adapters; i++)
        {
            VkPhysicalDeviceProperties props;
            func2(adapters[i], &props);
            memset(&info_arr[i], 0, sizeof(AdapterInfo));
            info_arr[i].iSize = sizeof(AdapterInfo);
            info_arr[i].iAdapterIndex = i;
            info_arr[i].iVendorID = convert_to_base_10((int)props.vendorID);
            info_arr[i].iExist = true;
            // info_arr[i].iBusNumber = convert_to_base_10((int)props.deviceID);

            //both lengths are statically allocated and are the same length, safe operation
            strcpy(info_arr[i].strAdapterName, props.deviceName);
        }

        free(adapters);
        return ADL_OK;
    }

    int __stdcall ADL2_Adapter_AdapterInfoX2_Get(ADL_CONTEXT_HANDLE context, AdapterInfo **info)
    {
        ADL_CONTEXT* adl_context = (ADL_CONTEXT*)context;
        ADL_LOCK();

        if (!info)
            return ADL_ERR_INVALID_PARAM;

        int num_adapters = 0;
        ADL2_Adapter_NumberOfAdapters_Get(context, &num_adapters);
        *info = (AdapterInfo*)adl_context->adl_malloc(sizeof(AdapterInfo) * num_adapters);
        if(!*info)
            return ADL_ERR;
        memset(*info, 0, sizeof(AdapterInfo) * num_adapters);

        return ADL2_Adapter_AdapterInfo_Get(context, *info, sizeof(AdapterInfo) * num_adapters);
    }
}
