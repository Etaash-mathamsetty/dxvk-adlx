#include "adlx_common.h"

extern "C"
{
    int __stdcall ADL2_Display_DisplayMapConfig_Get(ADL_CONTEXT_HANDLE context, int index, int *num_display_maps, ADLDisplayMap **display_maps,
                                                        int *num_display_target, ADLDisplayTarget **display_target, int options)
    {
        ADL_CONTEXT* adl_context = (ADL_CONTEXT*) context;
        ADL_LOCK();



        return ADL_OK;
    }
}
