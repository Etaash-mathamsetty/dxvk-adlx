#include "adlx_common.h"


extern "C"
{
    int __stdcall ADL2_Graphics_Platform_Get(ADL_CONTEXT_HANDLE context, int *platform)
    {
        if(!platform)
            return ADL_ERR_INVALID_PARAM;

        printf("FIXME: ADL2_Graphics_Platform_Get stub!\n");

        *platform = GRAPHICS_PLATFORM_DESKTOP;

        return ADL_OK;
    }

    int __stdcall ADL_Graphics_Platform_Get(int *platform)
    {
        return ADL2_Graphics_Platform_Get((ADL_CONTEXT_HANDLE)&global_adl_context, platform);
    }

    int __stdcall ADL2_Graphics_IsGfx9AndAbove(ADL_CONTEXT_HANDLE context)
    {
        printf("FIXME: ADL2_Graphics_IsGfx9AndAbove stub!\n");
        return TRUE;
    }

    int __stdcall ADL2_Graphics_Versions_Get(ADL_CONTEXT_HANDLE context, ADLVersionsInfo *info)
    {
        if(!info)
            return ADL_ERR_INVALID_PARAM;

        memcpy(info, &global_versions_info, sizeof(ADLVersionsInfo));

        return ADL_OK;
    }

    int __stdcall ADL2_Graphics_VersionsX2_Get(ADL_CONTEXT_HANDLE context, ADLVersionsInfoX2 *info)
    {
        if(!info)
            return ADL_ERR_INVALID_PARAM;

        memcpy(info, &global_versions_infox2, sizeof(ADLVersionsInfoX2));

        return ADL_OK;
    }

    int __stdcall ADL2_Graphics_VersionsX3_Get(ADL_CONTEXT_HANDLE context, int adapter_index, ADLVersionsInfoX2 *info)
    {
        if(!info)
            return ADL_ERR_INVALID_PARAM;

        printf("FIXME: Ignoring adapter_index %d\n", adapter_index);

        return ADL2_Graphics_VersionsX2_Get(context, info);
    }
}
