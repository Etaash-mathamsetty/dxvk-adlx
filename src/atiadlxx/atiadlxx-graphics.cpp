#include "adlx_common.h"


extern "C"
{
    int DLLEXPORT ADL2_Graphics_Platform_Get(ADL_CONTEXT_HANDLE context, int *platform)
    {
        ADL_CONTEXT* adl_context = (ADL_CONTEXT*) context;
        if(!platform)
            return ADL_ERR_INVALID_PARAM;

        print(adl_context, "FIXME: ADL2_Graphics_Platform_Get stub!\n");

        *platform = GRAPHICS_PLATFORM_DESKTOP;

        return ADL_OK;
    }

    int DLLEXPORT ADL_Graphics_Platform_Get(int *platform)
    {
        return ADL2_Graphics_Platform_Get((ADL_CONTEXT_HANDLE)&global_adl_context, platform);
    }

    int DLLEXPORT ADL2_Graphics_IsGfx9AndAbove(ADL_CONTEXT_HANDLE context)
    {
        ADL_CONTEXT* adl_context = (ADL_CONTEXT*) context;
        print(adl_context, "FIXME: ADL2_Graphics_IsGfx9AndAbove stub!\n");
        return TRUE;
    }

    int DLLEXPORT ADL2_Graphics_Versions_Get(ADL_CONTEXT_HANDLE context, ADLVersionsInfo *info)
    {
        if(!info)
            return ADL_ERR_INVALID_PARAM;

        memcpy(info, &global_versions_info, sizeof(ADLVersionsInfo));

        return ADL_OK;
    }

    int DLLEXPORT ADL_Graphics_Versions_Get(ADLVersionsInfo *info)
    {
        return ADL2_Graphics_Versions_Get((ADL_CONTEXT_HANDLE)&global_adl_context, info);
    }

    int DLLEXPORT ADL2_Graphics_VersionsX2_Get(ADL_CONTEXT_HANDLE context, ADLVersionsInfoX2 *info)
    {
        if(!info)
            return ADL_ERR_INVALID_PARAM;

        memcpy(info, &global_versions_infox2, sizeof(ADLVersionsInfoX2));

        return ADL_OK;
    }

    int DLLEXPORT ADL2_Graphics_VersionsX3_Get(ADL_CONTEXT_HANDLE context, int adapter_index, ADLVersionsInfoX2 *info)
    {
        ADL_CONTEXT *adl_context = (ADL_CONTEXT*) context;
        if(!info)
            return ADL_ERR_INVALID_PARAM;

        print(adl_context, std::string("FIXME: Ignoring adapter_index") + std::to_string(adapter_index) + "\n");

        return ADL2_Graphics_VersionsX2_Get(context, info);
    }
}
