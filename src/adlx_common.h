#include "adlx_private.h"
#include "dxvk/dxvk_interfaces.h"
#include "util/com_pointer.h"
#include "../version.h"
#include <sstream>

using PFN_wineDbgOutput = int(__cdecl*)(const char*);

inline PFN_wineDbgOutput wine_dbg;
inline std::ofstream log_file;

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
};

static void print(std::string message)
{
    if(wine_dbg)
        wine_dbg(message.c_str());
    else
        std::cerr << message;

    if(log_file.is_open())
        log_file << message;

    log_file.flush();
}

static std::string iid_to_string(REFIID iid)
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    ss << std::setw(8) << iid.Data1 << "-";
    ss << std::setw(4) << iid.Data2 << "-";
    ss << std::setw(4) << iid.Data3 << "-";
    ss << std::setw(2) << (int)iid.Data4[0];
    ss << std::setw(2) << (int)iid.Data4[1];
    ss << "-";
    ss << std::setw(2) << (int)iid.Data4[2];
    ss << std::setw(2) << (int)iid.Data4[3];
    ss << std::setw(2) << (int)iid.Data4[4];
    ss << std::setw(2) << (int)iid.Data4[5];
    ss << std::setw(2) << (int)iid.Data4[6];
    ss << std::setw(2) << (int)iid.Data4[7];
    return ss.str();
}

inline ADL_CONTEXT global_adl_context;

static const ADLVersionsInfo global_versions_info =
{
    "23.19.02-230831a-396538C-AMD-Software-Adrenalin-Edition",
    "23.10", //BF4 reads this version
    "http://support.amd.com/drivers/xml/driver_09_us.xml",
};

static const ADLVersionsInfoX2 global_versions_infox2 =
{
    "23.19.02-230831a-396538C-AMD-Software-Adrenalin-Edition",
    "23.10", //BF4 reads this version
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
