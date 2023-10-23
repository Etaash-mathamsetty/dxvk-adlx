adlx_src = files([
    'dxvk/dxvk_interfaces.cpp',
    'atiadlxx/atiadlxx.cpp',
])

adlx_dll = shared_library('atiadlxx', [ adlx_src, dxvk_adlx_version ],
    name_prefix         : '',
    dependencies        : [ lib_dxgi, lib_d3d11, lib_version ],
    include_directories : [ vk_headers ],
    install             : true)
