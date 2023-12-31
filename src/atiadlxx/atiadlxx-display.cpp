#include "../adlx_common.h"

extern "C"
{
    using namespace dxvk;

    int DLLEXPORT ADL2_Display_DisplayMapConfig_Get(ADL_CONTEXT_HANDLE context, int adapter_index, int *num_display_maps, ADLDisplayMap **display_maps,
                                                        int *num_display_target, ADLDisplayTarget **display_target, int options)
    {
        ADL_CONTEXT* adl_context = (ADL_CONTEXT*) context;
        ADL_LOCK();

        print( "FIXME: ADL2_Display_DisplayMapConfig_Get semi-stub\n");

        if(!num_display_maps || !display_maps || !num_display_target || !display_target)
            return ADL_ERR_INVALID_PARAM;

        if(adapter_index == -1)
        {
            printf("FIXME: ADL2_Display_DisplayMapConfig_Get adapter_index %d, using 0\n", adapter_index);
            adapter_index = 0;
        }

        if(options != 0)
        {
            printf("FIXME: ADL2_Display_DisplayMapConfig_Get Unimplemented options: %x\n", options);
        }

        int num_monitors = adl_context->monitor_count;

        *num_display_maps = num_monitors;
        *num_display_target = num_monitors;

        *display_maps = (ADLDisplayMap*)adl_context->adl_malloc(sizeof(ADLDisplayMap) * num_monitors);
        *display_target = (ADLDisplayTarget*)adl_context->adl_malloc(sizeof(ADLDisplayTarget) * num_monitors);

        DISPLAY_DEVICEA display_device;

        DEVMODEA dev_mode;
        memset(&dev_mode, 0, sizeof(DEVMODEA));
        dev_mode.dmSize = sizeof(DEVMODEA);

        for(int i = 0; i < num_monitors; i++)
        {
            ADLDisplayMap& map = (*display_maps)[i];
            ADLDisplayTarget& target = (*display_target)[i];
            ADLMode mode;

            map.iDisplayMapIndex = i;
            map.iNumDisplayTarget = 1;
            map.iFirstDisplayTargetArrayIndex = i;
            WINBOOL ret = EnumDisplayDevicesA(nullptr, i, &display_device, 0);

            ret = EnumDisplaySettingsA(display_device.DeviceName, ENUM_CURRENT_SETTINGS, &dev_mode);

            mode.displayID.iDisplayLogicalAdapterIndex = adapter_index;
            mode.displayID.iDisplayLogicalIndex = i;
            mode.displayID.iDisplayPhysicalAdapterIndex = adapter_index;
            mode.displayID.iDisplayPhysicalIndex = i;

            mode.fRefreshRate = (float)dev_mode.dmDisplayFrequency;
            mode.iColourDepth = dev_mode.dmBitsPerPel;
            mode.iAdapterIndex = adapter_index;
            mode.iXPos = dev_mode.dmPosition.x;
            mode.iYPos = dev_mode.dmPosition.y;
            mode.iXRes = dev_mode.dmPelsWidth;
            mode.iYRes = dev_mode.dmPelsHeight;
            mode.iOrientation = dev_mode.dmOrientation * 90; //DMDO_DEFAULT = 0, DMDO_90 = 1, DMDO_180 = 2, DMDO_270 = 3
            mode.iModeFlag = ADL_DISPLAY_MODE_PROGRESSIVE_FLAG;
            //FIXME
            mode.iModeMask = ADL_DISPLAY_MODE_COLOURFORMAT_8888 | ADL_DISPLAY_MODE_ORIENTATION_SUPPORTED_000 | ADL_DISPLAY_MODE_REFRESHRATE_ROUNDED;
            mode.iModeValue = ADL_DISPLAY_MODE_COLOURFORMAT_8888 | ADL_DISPLAY_MODE_ORIENTATION_SUPPORTED_000 | ADL_DISPLAY_MODE_REFRESHRATE_ROUNDED;
            map.displayMode = mode;
            map.iDisplayMapMask = ADL_DISPLAY_DISPLAYMAP_MANNER_SINGLE;
            map.iDisplayMapValue = ADL_DISPLAY_DISPLAYMAP_MANNER_SINGLE;

            target.displayID.iDisplayLogicalAdapterIndex = adapter_index;
            target.displayID.iDisplayLogicalIndex = i;
            target.displayID.iDisplayPhysicalAdapterIndex = adapter_index;
            target.displayID.iDisplayPhysicalIndex = i;

            target.iDisplayTargetMask = ADL_DISPLAY_DISPLAYTARGET_PREFERRED;
            target.iDisplayTargetValue = i == 0 ? ADL_DISPLAY_DISPLAYTARGET_PREFERRED : 0;
            target.iDisplayMapIndex = i;
        }

        print("TRACE: ADL2_Display_DisplayMapConfig_Get Success!");

        return ADL_OK;
    }

    int DLLEXPORT ADL2_Display_Modes_Get(ADL_CONTEXT_HANDLE context, int adapter_index, int display_index, int *num_modes, ADLMode **modes)
    {
        ADL_CONTEXT* adl_context = (ADL_CONTEXT*) context;
        ADL_LOCK();

        print( "TRACE: ADL2_Display_Modes_Get\n");

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
            print( "ERROR: ADL2_Display_Modes_Get EnumAdapters failed\n");
            return ADL_ERR;
        }

        res = adapter->EnumOutputs(display_index, (IDXGIOutput**)&output);

        if(FAILED(res))
        {
            print( "ERROR: ADL2_Display_Modes_Get EnumOutputs failed\n");
            return ADL_ERR;
        }

        uint32_t num_modes_dxgi = 0;

        res = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &num_modes_dxgi, nullptr);

        if(FAILED(res))
        {
            print( "ERROR: ADL2_Display_Modes_Get GetDisplayModeList failed\n");
            return ADL_ERR;
        }

        DXGI_MODE_DESC *modes_dxgi = (DXGI_MODE_DESC*)alloca(num_modes_dxgi * sizeof(DXGI_MODE_DESC));

        res = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &num_modes_dxgi, modes_dxgi);

        if(FAILED(res))
        {
            print( "ERROR: ADL2_Display_Modes_Get GetDisplayModeList failed\n");
            return ADL_ERR;
        }

        *num_modes = num_modes_dxgi;

        *modes = (ADLMode*)adl_context->adl_malloc(num_modes_dxgi * sizeof(ADLMode));

        if(!*modes)
        {
            print( "ERROR: ADL2_Display_Modes_Get adl_malloc failed\n");
            return ADL_ERR;
        }

        memset(*modes, 0, num_modes_dxgi * sizeof(ADLMode));

        DISPLAY_DEVICEA display_device;

        WINBOOL ret = EnumDisplayDevicesA(nullptr, display_index, &display_device, 0);

        if(!ret)
        {
            print( "ERROR: ADL2_Display_Modes_Get EnumDisplayDevicesA failed\n");
            return ADL_ERR;
        }

        DEVMODEA dev_mode;
        memset(&dev_mode, 0, sizeof(DEVMODEA));
        dev_mode.dmSize = sizeof(DEVMODEA);

        ret = EnumDisplaySettingsA(display_device.DeviceName, ENUM_CURRENT_SETTINGS, &dev_mode);

        if(!ret)
        {
            print( "ERROR: ADL2_Display_Modes_Get EnumDisplaySettingsA failed\n");
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
            mode.iModeFlag = ADL_DISPLAY_MODE_PROGRESSIVE_FLAG;
            //FIXME
            mode.iModeMask = ADL_DISPLAY_MODE_COLOURFORMAT_8888 | ADL_DISPLAY_MODE_ORIENTATION_SUPPORTED_000 | ADL_DISPLAY_MODE_REFRESHRATE_ROUNDED;
            mode.iModeValue = ADL_DISPLAY_MODE_COLOURFORMAT_8888 | ADL_DISPLAY_MODE_ORIENTATION_SUPPORTED_000 | ADL_DISPLAY_MODE_REFRESHRATE_ROUNDED;
            mode.displayID = { display_index, display_index, adapter_index, adapter_index };
        }

        return ADL_OK;
    }

    int DLLEXPORT ADL_Display_Modes_Get(int adapter_index, int display_index, int *num_modes, ADLMode **modes)
    {
        return ADL2_Display_Modes_Get((ADL_CONTEXT_HANDLE)&global_adl_context, adapter_index, display_index, num_modes, modes);
    }

    int DLLEXPORT ADL_Display_DisplayMapConfig_Get(int index, int *num_display_maps, ADLDisplayMap **display_maps,
                                                        int *num_display_target, ADLDisplayTarget **display_target, int options)
    {
        print("TRACE: ADL_Display_DisplayMapConfig_Get\n");
        return ADL2_Display_DisplayMapConfig_Get((ADL_CONTEXT_HANDLE)&global_adl_context, index, num_display_maps, display_maps,
                                                        num_display_target, display_target, options);
    }

    int DLLEXPORT ADL2_Display_SLSMapIndex_Get(ADL_CONTEXT_HANDLE context, int adapter_index, int display_target, ADLDisplayTarget *display_target_map, int *sls_map_index)
    {
        print("TRACE: ADL2_Display_SLSMapIndex_Get\n");

        //docs say -1 not supported for this one (yay)
        if(adapter_index < 0 || !display_target_map || display_target <= 0 || !sls_map_index)
        {
            return ADL_ERR_INVALID_PARAM;
        }

        //assume there is only one SLS map
        *sls_map_index = 0;

        return ADL_OK;
    }

    int DLLEXPORT ADL_Display_SLSMapIndex_Get(int adapter_index, int display_target, ADLDisplayTarget *display_target_map_index, int *sls_map_index)
    {
        return ADL2_Display_SLSMapIndex_Get((ADL_CONTEXT_HANDLE)&global_adl_context, adapter_index, display_target,
                                                display_target_map_index, sls_map_index);
    }

    int DLLEXPORT ADL2_Display_SLSMapConfig_Get(     ADL_CONTEXT_HANDLE  	context,
        int  	iAdapterIndex,
		int  	iSLSMapIndex,
		ADLSLSMap *  	lpSLSMap,
		int *  	lpNumSLSTarget,
		ADLSLSTarget **  	lppSLSTarget,
		int *  	lpNumNativeMode,
		ADLSLSMode **  	lppNativeMode,
		int *  	lpNumBezelMode,
		ADLBezelTransientMode **  	lppBezelMode,
		int *  	lpNumTransientMode,
		ADLBezelTransientMode **  	lppTransientMode,
		int *  	lpNumSLSOffset,
		ADLSLSOffset **  	lppSLSOffset,
		int  	iOption )
    {
        print("FIXME: ADL2_Display_SLSMapConfig_Get stub\n");

        if(iSLSMapIndex != 0)
        {
            return ADL_ERR_INVALID_PARAM;
        }

        return ADL_ERR;
    }

    int DLLEXPORT ADL_Display_SLSMapConfig_Get( 	int  	iAdapterIndex,
		int  	iSLSMapIndex,
		ADLSLSMap *  	lpSLSMap,
		int *  	lpNumSLSTarget,
		ADLSLSTarget **  	lppSLSTarget,
		int *  	lpNumNativeMode,
		ADLSLSMode **  	lppNativeMode,
		int *  	lpNumBezelMode,
		ADLBezelTransientMode **  	lppBezelMode,
		int *  	lpNumTransientMode,
		ADLBezelTransientMode **  	lppTransientMode,
		int *  	lpNumSLSOffset,
		ADLSLSOffset **  	lppSLSOffset,
		int  	iOption )
    {
        return ADL2_Display_SLSMapConfig_Get( (ADL_CONTEXT_HANDLE)&global_adl_context, iAdapterIndex, iSLSMapIndex, lpSLSMap,
                                                lpNumSLSTarget, lppSLSTarget, lpNumNativeMode, lppNativeMode, lpNumBezelMode,
                                                    lppBezelMode, lpNumTransientMode, lppTransientMode, lpNumSLSOffset,
                                                    lppSLSOffset, iOption );
    }

    int DLLEXPORT ADL2_Display_FreeSync_Cap(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, ADLFreeSyncCap* cap)
    {
        ADL_CONTEXT* adl_context = (ADL_CONTEXT*) context;
        print( "FIXME: ADL2_Display_FreeSync_Cap stub, faking FreeSync support!\n");

        if(!cap)
            return ADL_ERR_INVALID_PARAM;

        if(iAdapterIndex == -1)
        {
            printf("FIXME: ADL2_Display_FreeSync_Cap iAdapterIndex %d, using 0\n", iAdapterIndex);
            iAdapterIndex = 0;
        }

        if(iDisplayIndex == -1)
        {
            printf("FIXME: ADL2_Display_FreeSync_Cap iDisplayIndex %d, using 0\n", iDisplayIndex);
            iDisplayIndex = 0;
        }

        DISPLAY_DEVICEA display_device;

        WINBOOL ret = EnumDisplayDevicesA(nullptr, iDisplayIndex, &display_device, 0);

        DEVMODEA dev_mode;
        memset(&dev_mode, 0, sizeof(DEVMODEA));
        dev_mode.dmSize = sizeof(DEVMODEA);

        ret = EnumDisplaySettingsA(display_device.DeviceName, ENUM_CURRENT_SETTINGS, &dev_mode);

        cap->iCaps = ADL_FREESYNC_CAP_SUPPORTED | ADL_FREESYNC_CAP_CURRENTMODESUPPORTED | ADL_FREESYNC_CAP_GPUSUPPORTED;
        cap->iMaxRefreshRateInMicroHz = dev_mode.dmDisplayFrequency * 1000000;
        cap->iMinRefreshRateInMicroHz = dev_mode.dmDisplayFrequency * 1000000;
        cap->ucLabelIndex = ADL_FREESYNC_LABEL_FREESYNC_PREMIUM_PRO;

        return ADL_OK;
    }

    int DLLEXPORT ADL2_Display_DisplayInfo_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* numDisplays, ADLDisplayInfo** infos, int force_detect)
    {
        ADL_CONTEXT* adl_context = (ADL_CONTEXT*) context;
        print("FIXME: ADL2_Display_DisplayInfo_Get ignoring adapter index, force_detect, semi-stub\n");

        int num_monitors = adl_context->monitor_count;

        if(!numDisplays || !infos)
            return ADL_ERR_INVALID_PARAM;

        if(iAdapterIndex == -1)
            iAdapterIndex = 0;

        *numDisplays = num_monitors;

        DEVMODEA dev_mode;
        memset(&dev_mode, 0, sizeof(DEVMODEA));
        dev_mode.dmSize = sizeof(DEVMODEA);

        *infos = (ADLDisplayInfo*)adl_context->adl_malloc(sizeof(ADLDisplayInfo) * num_monitors);

        DISPLAY_DEVICEA display_device;

        for(int i = 0; i < num_monitors; i++)
        {
            ADLDisplayInfo& info = (*infos)[i];
            info.displayID.iDisplayPhysicalAdapterIndex = iAdapterIndex;
            info.displayID.iDisplayLogicalAdapterIndex = iAdapterIndex;
            info.displayID.iDisplayLogicalIndex = i;
            info.displayID.iDisplayPhysicalIndex = i;

            WINBOOL ret = EnumDisplayDevicesA(nullptr, i, &display_device, 0);

            ret = EnumDisplaySettingsA(display_device.DeviceName, ENUM_CURRENT_SETTINGS, &dev_mode);

            strcpy(info.strDisplayName, display_device.DeviceString);
            //FIXME
            strcpy(info.strDisplayManufacturerName, "Samsung");
            info.iDisplayConnector = ADL_DISPLAY_CONTYPE_DISPLAYPORT;
            info.iDisplayControllerIndex = 0;
            info.iDisplayInfoMask = ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED | ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED;
            info.iDisplayInfoValue = ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED | ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED;
            //FIXME: what value here?
            info.iDisplayOutputType = 0;
            info.iDisplayType = ADL_DT_LCD_PANEL;
        }

        return ADL_OK;
    }

    int DLLEXPORT ADL_Display_DisplayInfo_Get(int iAdapterIndex, int* numDisplays, ADLDisplayInfo** infos, int force_detect)
    {
        return ADL2_Display_DisplayInfo_Get((ADL_CONTEXT_HANDLE)&global_adl_context, iAdapterIndex, numDisplays, infos, force_detect);
    }
}
