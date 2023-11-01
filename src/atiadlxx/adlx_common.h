#include "../adlx_private.h"
#include "../dxvk/dxvk_interfaces.h"
#include "../util/com_pointer.h"
#include "../../version.h"

using PFN_wineDbgOutput = int(__cdecl*)(const char*);

struct ADL_CONTEXT
{
    ADLThreadingModel threading_model;
    ADL_MAIN_MALLOC_CALLBACK adl_malloc;
    int enum_connected_adapters;
    dxvk::Com<IDXGIFactory> dxgi_factory;
    VkInstance vk_instance;
    PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr;
    //ADL2 might not behave identically to ADL1
    bool is_adl1;
    PFN_wineDbgOutput wine_dbg;
    std::ofstream log_file;
};

static void print(ADL_CONTEXT* ctx, std::string message)
{
    if(!ctx)
        return;

    if(ctx->wine_dbg)
        ctx->wine_dbg(message.c_str());
    else
        std::cerr << message;

    if(ctx->log_file.is_open())
        ctx->log_file << message;

    ctx->log_file.flush();
}

inline ADL_CONTEXT global_adl_context;

static const ADLVersionsInfo global_versions_info =
{
    //FIXME
    "23.10.23.02-230720a-394204C-AMD-Software-Adrenalin-Edition",
    "",
    "http://support.amd.com/drivers/xml/driver_09_us.xml",
};

static const ADLVersionsInfoX2 global_versions_infox2 =
{
    //FIXME
    "23.10.23.02-230720a-394204C-AMD-Software-Adrenalin-Edition",
    "",
    "23.10.2",
    "http://support.amd.com/drivers/xml/driver_09_us.xml",
};

/*
static auto adlMutex = std::mutex{};
#define ADL_LOCK() std::unique_lock<std::mutex> _lock____ \
  = (adl_context->threading_model == ADL_THREADING_LOCKED) ? \
  std::unique_lock<std::mutex>(adlMutex) : std::unique_lock<std::mutex>() \
*/

//no-op for now, since no known applications to test with
#define ADL_LOCK() do { } while(0)

static int convert_to_base_10(int id)
{
    char str[16];
    snprintf(str, 16, "%x", id);
    return atoi(str);
}

#define DLLEXPORT __stdcall __declspec(dllexport)