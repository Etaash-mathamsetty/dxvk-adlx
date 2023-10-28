#include "adlx_common.h"

extern "C"
{
    using namespace dxvk;

    int __stdcall ADL2_Display_DisplayMapConfig_Get(ADL_CONTEXT_HANDLE context, int index, int *num_display_maps, ADLDisplayMap **display_maps,
                                                        int *num_display_target, ADLDisplayTarget **display_target, int options)
    {
        ADL_CONTEXT* adl_context = (ADL_CONTEXT*) context;
        ADL_LOCK();



        return ADL_OK;
    }

    int __stdcall ADL2_Display_Modes_Get(ADL_CONTEXT_HANDLE context, int adapter_index, int display_index, int *num_modes, ADLMode **modes)
    {
        ADL_CONTEXT* adl_context = (ADL_CONTEXT*) context;
        ADL_LOCK();

        if(adapter_index == -1)
        {
            printf("FIXME: ADL2_Display_Modes_Get adapter_index %d, using 0\n", adapter_index);
            adapter_index = 0;
        }

        if(display_index == -1)
        {
            printf("FIXME: ADL2_Display_Modes_Get display_index %d, using 0\n", display_index);
            display_index = 0;
        }

        if(!num_modes || !modes)
            return ADL_ERR_INVALID_PARAM;

        Com<IDXGIAdapter> adapter;
        Com<IDXGIOutput> output;

        HRESULT res = adl_context->dxgi_factory->EnumAdapters(adapter_index, (IDXGIAdapter**)&adapter);

        if(FAILED(res))
        {
            printf("ERROR: ADL2_Display_Modes_Get EnumAdapters failed\n");
            return ADL_ERR;
        }

        res = adapter->EnumOutputs(display_index, (IDXGIOutput**)&output);

        if(FAILED(res))
        {
            printf("ERROR: ADL2_Display_Modes_Get EnumOutputs failed\n");
            return ADL_ERR;
        }

        uint32_t num_modes_dxgi = 0;

        res = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &num_modes_dxgi, nullptr);

        if(FAILED(res))
        {
            printf("ERROR: ADL2_Display_Modes_Get GetDisplayModeList failed\n");
            return ADL_ERR;
        }

        DXGI_MODE_DESC *modes_dxgi = (DXGI_MODE_DESC*)alloca(num_modes_dxgi * sizeof(DXGI_MODE_DESC));

        res = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &num_modes_dxgi, modes_dxgi);

        if(FAILED(res))
        {
            printf("ERROR: ADL2_Display_Modes_Get GetDisplayModeList failed\n");
            return ADL_ERR;
        }

        *num_modes = num_modes_dxgi;

        *modes = (ADLMode*)adl_context->adl_malloc(num_modes_dxgi * sizeof(ADLMode));

        if(!*modes)
        {
            printf("ERROR: ADL2_Display_Modes_Get adl_malloc failed\n");
            return ADL_ERR;
        }

        memset(*modes, 0, num_modes_dxgi * sizeof(ADLMode));

        DISPLAY_DEVICEA display_device;

        WINBOOL ret = EnumDisplayDevicesA(nullptr, display_index, &display_device, 0);

        if(!ret)
        {
            printf("ERROR: ADL2_Display_Modes_Get EnumDisplayDevicesA failed\n");
            return ADL_ERR;
        }

        DEVMODEA dev_mode;
        memset(&dev_mode, 0, sizeof(DEVMODEA));
        dev_mode.dmSize = sizeof(DEVMODEA);

        ret = EnumDisplaySettingsA(display_device.DeviceName, ENUM_CURRENT_SETTINGS, &dev_mode);

        if(!ret)
        {
            printf("ERROR: ADL2_Display_Modes_Get EnumDisplaySettingsA failed\n");
            return ADL_ERR;
        }

        for(uint32_t i = 0; i < num_modes_dxgi; i++)
        {
            ADLMode& mode = (*modes)[i];
            mode.fRefreshRate = (float)modes_dxgi[i].RefreshRate.Numerator /
                                                            (float)modes_dxgi[i].RefreshRate.Denominator;

            mode.iColourDepth = dev_mode.dmBitsPerPel;
            mode.iAdapterIndex = adapter_index;
            mode.iYRes = modes_dxgi[i].Height;
            mode.iXRes = modes_dxgi[i].Width;
            mode.iOrientation = dev_mode.dmOrientation * 90; //DMDO_DEFAULT = 0, DMDO_90 = 1, DMDO_180 = 2, DMDO_270 = 3
            mode.iXPos = dev_mode.dmPosition.x;
            mode.iYPos = dev_mode.dmPosition.y;
            mode.iModeFlag = ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED | ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED;
            mode.iModeMask = mode.iModeFlag;
            mode.displayID = { display_index, display_index, adapter_index, adapter_index };
        }

        return ADL_OK;
    }

    int __stdcall ADL_Display_Modes_Get(int adapter_index, int display_index, int *num_modes, ADLMode **modes)
    {
        return ADL2_Display_Modes_Get((ADL_CONTEXT_HANDLE)&global_adl_context, adapter_index, display_index, num_modes, modes);
    }

    int __stdcall ADL_Display_DisplayMapConfig_Get(int index, int *num_display_maps, ADLDisplayMap **display_maps,
                                                        int *num_display_target, ADLDisplayTarget **display_target, int options)
    {
        return ADL2_Display_DisplayMapConfig_Get((ADL_CONTEXT_HANDLE)&global_adl_context, index, num_display_maps, display_maps,
                                                        num_display_target, display_target, options);
    }
}
