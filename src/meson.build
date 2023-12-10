adlx_src = files([
    'dxvk/dxvk_interfaces.cpp',
    'atiadlxx/atiadlxx.cpp',
    'atiadlxx/atiadlxx-adapter.cpp',
    'atiadlxx/atiadlxx-graphics.cpp',
    'atiadlxx/atiadlxx-display.cpp',
])

amdxc_src = files([
    'amdxc/main.cpp',
])

adlx_dll = shared_library('atiadlx' + target_suffix, [ adlx_src, dxvk_adlx_version ],
    name_prefix         : '',
    dependencies        : [ lib_dxgi, lib_d3d11, lib_version ],
    include_directories : [ vk_headers ],
    install             : true,
    vs_module_defs      : 'atiadlxx/atiadlxx.def')

amdxc_src += shared_library('amdxc' + target_suffix2, [ amdxc_src, dxvk_adlx_version ],
    name_prefix         : '',
    dependencies        : [ lib_dxgi, lib_d3d11, lib_version ],
    include_directories : [ vk_headers ],
    install             : true,
    vs_module_defs      : 'amdxc/amdxc.def')
