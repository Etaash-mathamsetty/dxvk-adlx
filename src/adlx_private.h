#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <set>
#include <optional>
#include <sstream>
#include <fstream>
#include <ctime>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <dxgi1_6.h>
#include <d3d11_1.h>
#include "../inc/d3d12.h"
#include "../inc/adl_sdk.h"
#include <vulkan/vulkan.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Wattributes"
#endif // __GNUC__

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // __GNUC__

#if defined(__GNUC__)
#define _ReturnAddress() __builtin_return_address(0);
#endif